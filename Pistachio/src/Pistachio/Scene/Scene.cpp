#include "pstpch.h"

#include "Scene.h"

#include <glm/glm.hpp>

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

#include "Pistachio/Renderer/Renderer2D.h"
#include "Pistachio/Renderer/EditorCamera.h"

#include "Pistachio/Scene/Entity.h"
#include "Pistachio/Scene/ScriptableEntity.h"

#include "Pistachio/Scripting/ScriptEngine.h"

#include "Pistachio/Utils/Box2DUtils.h"


namespace Pistachio {

	template<typename... C>
	static void CopyComponents(entt::registry& dest, entt::registry& source, const std::unordered_map<UUID, entt::entity>& destEnttIDsByUUID) {
		([&]() {
			auto view = source.view<UUIDComponent, C>();
			for (auto&& [enttID, uuidComponent, component] : view.each()) {
				entt::entity destEnttID = destEnttIDsByUUID.at(uuidComponent.UUID);
				dest.emplace_or_replace<C>(destEnttID, component);
			}
		}(), ...);
	}

	template<typename... C>
	static void CopyComponentGroup(ComponentGroup<C...> componentGroup,
		entt::registry& dest, entt::registry& source, const std::unordered_map<UUID, entt::entity>& destEnttIDsByUUID) {
		CopyComponents<C...>(dest, source, destEnttIDsByUUID);
	}

	template<typename... C>
	static void CopyComponentsIfExists(Entity dest, Entity source) {
		([&]() {
			if (source.HasComponent<C>()) {
				dest.AddOrReplaceComponent<C>(source.Component<C>());
			}
		}(), ...);
	}

	template<typename... C>
	static void CopyComponentGroupIfExists(ComponentGroup<C...> componentGroup, Entity dest, Entity source) {
		CopyComponentsIfExists<C...>(dest, source);
	}


	Scene::Scene(const std::string& name /*= "Untitled"*/)
		: m_Name(name) {
	}

	Scene::~Scene() {
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other) {
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		std::unordered_map<UUID, entt::entity> destEnttIDsByUUID;

		auto& sourceSceneRegistery = other->m_Registry;
		auto& destSceneRegistery = newScene->m_Registry;

		for (auto& entity : other->m_Entities) {
			UUID uuid = entity.UUID();
			entt::entity destEnttID = newScene->CreateEntityWithUUID(uuid, entity.Name());
			destEnttIDsByUUID[uuid] = destEnttID;
		}

		CopyComponentGroup(AllComponents{}, destSceneRegistery, sourceSceneRegistery, destEnttIDsByUUID);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name /*= std::string()*/) {
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name) {
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<UUIDComponent>(uuid);
		entity.AddComponent<TagComponent>(name.empty() ? "Unnamed Entity" : name);
		entity.AddComponent<TransformComponent>();

		m_EntityMap[uuid] = entity;
		m_Entities.emplace_back(entity);

		return entity;
	}

	Entity Scene::DuplicateEntity(Entity entity) {
		Entity newEntity = CreateEntity(entity.Name());

		CopyComponentGroupIfExists(AllComponents{}, newEntity, entity);

		return newEntity;
	}

	void Scene::DestroyEntity(Entity entity) {
		m_EntityMap.erase(entity.UUID());
		m_Entities.erase(std::remove(m_Entities.begin(), m_Entities.end(), entity), m_Entities.end());

		m_Registry.destroy(entity);
	}

	int Scene::EntityPosition(Entity entity) {
		auto pos = std::find(m_Entities.begin(), m_Entities.end(), entity);
		if (pos == m_Entities.end()) {
			return -1;
		}
		return pos - m_Entities.begin();
	}

	void Scene::MoveEntityToEntityPosition(Entity moveEntity, Entity destEntity) {
		int oldPosition = EntityPosition(moveEntity);
		int newPosition = EntityPosition(destEntity);
		if (oldPosition == -1 || newPosition == -1) {
			return;
		}

		if (newPosition > oldPosition) {
			std::rotate(m_Entities.begin() + oldPosition, m_Entities.begin() + oldPosition + 1, m_Entities.begin() + newPosition + 1);
		} else {
			std::rotate(m_Entities.rend() - oldPosition - 1, m_Entities.rend() - oldPosition, m_Entities.rend() - newPosition);
		}
	}

	Entity Scene::EntityByUUID(UUID uuid) {
		auto& entity = m_EntityMap.find(uuid);
		// TODO: Maybe should be an assert
		if (entity == m_EntityMap.end()) {
			return Entity();
		}
		return { entity->second, this };
	}

	b2Body* Scene::RigidBodyByUUID(UUID uuid) {
		auto& rigidbody = m_RuntimeBodies.find(uuid);
		// TODO: Maybe should be an assert
		if (rigidbody == m_RuntimeBodies.end()) {
			return nullptr;
		}
		return rigidbody->second;
	}

	void Scene::OnRuntimeStart() {
		// Initialise Physics world
		Init2DPhysics();


		// Instantiate Native Scripts
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto&& [entity, script] : view.each()) {
				script.ScriptInstance = script.CreateScriptInstance();
				script.ScriptInstance->m_Entity = Entity(entity, this);
				script.ScriptInstance->OnCreate();
			}
		}


		// C# Scripts On Runtime Start
		{
			ScriptEngine::OnRuntimeStart(this);

			// Instantiate all Script entities
			auto view = m_Registry.view<ScriptComponent>();
			for (auto& enttID : view) {
				ScriptEngine::EntityCreateClassInstance({ enttID, this });
			}
		}


		// Update cameras
		OnViewportResize(m_ViewportWidth, m_ViewportHeight);
	}

	void Scene::OnRuntimeStop() {
		// C# Scripts On Runtime Stop
		ScriptEngine::OnRuntimeStop();

		// Destroy Script Instance
		auto view = m_Registry.view<NativeScriptComponent>();
		for (auto&& [enttID, script] : view.each()) {
			script.ScriptInstance->OnDestroy();
		}


		// Destroy physics world
		Destroy2DPhysics();
	}

	void Scene::OnSimulationStart() {
		Init2DPhysics();
	}

	void Scene::OnSimulationStop() {
		Destroy2DPhysics();
	}

	void Scene::OnUpdateEditor(Timestep timestep, EditorCamera& camera) {
		Renderer2D::BeginScene(camera);

		RenderScene();

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateRuntime(Timestep timestep) {
		// Update Physics 2D
		Update2DPhysics(timestep);


		// Update Internal Scripts
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto&& [enttID, script] : view.each()) {
				script.ScriptInstance->OnUpdate(timestep);
			}
		}


		// Update C# Scripts
		{
			auto view = m_Registry.view<ScriptComponent>();
			for (auto enttID : view) {
				ScriptEngine::EntityOnUpdate({ enttID, this }, timestep);
			}
		}


		// Render 2D 
		glm::mat4 cameraTransform;
		Camera* mainCamera = nullptr;  // used as a weakref
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto&& [enttID, transform, camera] : view.each()) {
				if (camera.Primary) {
					cameraTransform = transform.Transform();
					mainCamera = &camera.Camera;
					break;
				}
			}
		}

		if (!mainCamera) return;

		Renderer2D::BeginScene(*mainCamera, cameraTransform);

		RenderScene();

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateSimulation(Timestep timestep, EditorCamera& camera) {
		Update2DPhysics(timestep);

		Renderer2D::BeginScene(camera);
		RenderScene();
		Renderer2D::EndScene();
	}

	void Scene::OnViewportResize(unsigned int width, unsigned int height) {
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize non-fixed aspect ratio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto&& [entity, camera] : view.each()) {
			if (!camera.FixedAspectRatio) {
				camera.Camera.SetViewportSize(width, height);
			}
		}
	}

	void Scene::EachEntity(std::function<void(Entity)> callback) {
		for (auto& entity : m_Entities) {
			callback(entity);
		}
	}

	void Scene::Init2DPhysics() {
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<TransformComponent, RigidBody2DComponent>();
		for (auto&& [enntID, transform, rigidBody] : view.each()) {
			Entity entity = { enntID, this };

			b2BodyDef bodyDef;
			bodyDef.type = Utils::RigidBody2DTypeToBox2DType(rigidBody.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rigidBody.FixedRotation);

			m_RuntimeBodies[entity.UUID()] = body;

			if (entity.HasComponent<BoxCollider2DComponent>()) {
				auto& boxCollider = entity.Component<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(
					transform.Scale.x * 0.5f * boxCollider.Size.x,
					transform.Scale.y * 0.5f * boxCollider.Size.y,
					{ boxCollider.Offset.x, boxCollider.Offset.y },
					0.0f
				);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = boxCollider.Density;
				fixtureDef.friction = boxCollider.Friction;
				fixtureDef.restitution = boxCollider.Restitution;
				fixtureDef.restitutionThreshold = boxCollider.RestitutionThreshold;

				b2Fixture* fixture = body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>()) {
				auto& circleCollider = entity.Component<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(circleCollider.Offset.x, circleCollider.Offset.y);
				circleShape.m_radius = transform.Scale.x * circleCollider.Radius;  // Only respects x-axis scale

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = circleCollider.Density;
				fixtureDef.friction = circleCollider.Friction;
				fixtureDef.restitution = circleCollider.Restitution;
				fixtureDef.restitutionThreshold = circleCollider.RestitutionThreshold;

				b2Fixture* fixture = body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::Destroy2DPhysics() {
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;

		m_RuntimeBodies.clear();
	}

	void Scene::Update2DPhysics(Timestep timestep) {
		// TODO: Tweak / expose to editor
		const int velocityIteration = 6;
		const int positionIteration = 2;

		m_PhysicsWorld->Step(timestep, velocityIteration, positionIteration);

		// Retrieve transform from Box2D
		auto view = m_Registry.view<TransformComponent, RigidBody2DComponent>();
		for (auto&& [enntID, transform, rigidBody] : view.each()) {
			Entity entity = { enntID, this };

			b2Body* body = m_RuntimeBodies[entity.UUID()];

			const auto& position = body->GetPosition();
			transform.Translation.x = position.x;
			transform.Translation.y = position.y;

			transform.Rotation.z = body->GetAngle();
		}
	}

	void Scene::RenderScene() {
		// Sprites
		{
			auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
			for (auto&& [enttID, transform, spriteComponent] : view.each()) {
				Renderer2D::DrawSprite(transform.Transform(), spriteComponent.Sprite, (int)enttID);
			}
		}

		// Circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto&& [enttID, transform, circleComponent] : view.each()) {
				Renderer2D::DrawCircle(transform.Transform(), circleComponent.Colour, circleComponent.Thickness, circleComponent.Blur, (int)enttID);
			}
		}
	}

	Entity Scene::PrimaryCameraEntity() {
		auto view = m_Registry.view<CameraComponent>();
		for (auto&&[enttID, cameraComponent] : view.each()) {
			if (cameraComponent.Primary) {
				return Entity(enttID, this);
			}
		}
		return Entity();
	}

	// FIXME: Whyyy any of this 
	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component) {
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded(Entity entity, UUIDComponent& component) {

	}
	template void Scene::OnComponentAdded<UUIDComponent>(Entity entity, UUIDComponent& component);

	template<>
	void Scene::OnComponentAdded(Entity entity, TagComponent& component) {

	}
	template void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component);

	template<>
	void Scene::OnComponentAdded(Entity entity, TransformComponent& component) {

	}
	template void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component);

	template<>
	void Scene::OnComponentAdded(Entity entity, SpriteRendererComponent& component) {

	}
	template void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component);

	template<>
	void Scene::OnComponentAdded(Entity entity, CircleRendererComponent& component) {

	}
	template void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component);

	template<>
	void Scene::OnComponentAdded(Entity entity, CameraComponent& component) {
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
	template void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component);

	template<>
	void Scene::OnComponentAdded(Entity entity, NativeScriptComponent& component) {

	}
	template void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component);

	template<>
	void Scene::OnComponentAdded(Entity entity, ScriptComponent& component) {

	}
	template void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component);

	template<>
	void Scene::OnComponentAdded(Entity entity, RigidBody2DComponent& component) {

	}
	template void Scene::OnComponentAdded<RigidBody2DComponent>(Entity entity, RigidBody2DComponent& component);

	template<>
	void Scene::OnComponentAdded(Entity entity, BoxCollider2DComponent& component) {

	}
	template void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component);

	template<>
	void Scene::OnComponentAdded(Entity entity, CircleCollider2DComponent& component) {

	}
	template void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component);

}
