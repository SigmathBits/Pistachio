#include "pstpch.h"

#include "Scene.h"

#include <glm/glm.hpp>

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

#include "Pistachio/Renderer/Renderer2D.h"
#include "Pistachio/Renderer/EditorCamera.h"

#include "Pistachio/Scene/Entity.h"
#include "Pistachio/Scene/ScriptableEntity.h"


namespace Pistachio {

	static b2BodyType RigidBody2DTypeToBox2DType(RigidBody2DComponent::BodyType bodyType) {
		switch (bodyType) {
			case RigidBody2DComponent::BodyType::Static:
				return b2_staticBody;
				break;
			case RigidBody2DComponent::BodyType::Dynamic:
				return b2_dynamicBody;
				break;
			case RigidBody2DComponent::BodyType::Kinematic:
				return b2_kinematicBody;
				break;
			default:
				break;
		}

		PST_CORE_ASSERT(false, "Unrecognised body type");
		return b2_staticBody;
	}

	Scene::Scene() {

	}

	Scene::~Scene() {
	}

	Entity Scene::CreateEntity(const std::string& name /*= std::string()*/) {
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name) {
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TagComponent>(name.empty() ? "Unnamed Entity" : name);
		entity.AddComponent<TransformComponent>();
		return entity;
	}

	void Scene::DestroyEntity(Entity entity) {
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart() {
		// Instantiate Scripts
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& script) {
			script.ScriptInstance = script.CreateScriptInstance();
			script.ScriptInstance->m_Entity = Entity(entity, this);
			script.ScriptInstance->OnCreate();
		});


		// Initialise Physics world
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });
		
		auto group = m_Registry.group<RigidBody2DComponent>(entt::get<TransformComponent>);
		for (auto&& [e, rigidBody, transform] : group.each()) {
			Entity entity = { e, this };

			b2BodyDef bodyDef;
			bodyDef.type = RigidBody2DTypeToBox2DType(rigidBody.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rigidBody.FixedRotation);

			m_RuntimeBodies[entity.ID()] = body;

			if (!entity.HasComponent<BoxCollider2DComponent>()) continue;

			auto& boxCollider = entity.Component<BoxCollider2DComponent>();

			b2PolygonShape boxShape;
			boxShape.SetAsBox(
				transform.Scale.x * 0.5f * boxCollider.Size.x, 
				transform.Scale.y * 0.5f * boxCollider.Size.y
			);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &boxShape;
			fixtureDef.density = boxCollider.Density;
			fixtureDef.friction = boxCollider.Friction;
			fixtureDef.restitution = boxCollider.Restitution;
			fixtureDef.restitutionThreshold = boxCollider.RestitutionThreshold;

			b2Fixture* fixture = body->CreateFixture(&fixtureDef);
		}
	}

	void Scene::OnRuntimeStop() {
		// Destroy Script Instance
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& script) {
			script.ScriptInstance->OnDestroy();
		});


		// Destroy physics world
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;

		m_RuntimeBodies.clear();
	}

	void Scene::OnUpdateEditor(Timestep timestep, EditorCamera& camera) {
		Renderer2D::BeginScene(camera);

		{
			auto group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
			for (auto&& [entity, spriteComponent, transform] : group.each()) {
				Renderer2D::DrawSprite(transform.Transform(), spriteComponent.Sprite, (int)entity);
			}
		}

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateRuntime(Timestep timestep) {
		// Update Scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& script) {
				script.ScriptInstance->OnUpdate(timestep);
			});
		}


		// Physics
		{
			// TODO: Tweak / expose to editor
			const int velocityIteration = 6;
			const int positionIteration = 2;

			m_PhysicsWorld->Step(timestep, velocityIteration, positionIteration);

			// Retrieve transform from Box2D
			auto group = m_Registry.group<RigidBody2DComponent>(entt::get<TransformComponent>);
			for (auto&& [e, rigidBody, transform] : group.each()) {
				Entity entity = { e, this };

				b2Body* body = m_RuntimeBodies[entity.ID()];

				const auto& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;

				transform.Rotation.z = body->GetAngle();
			}
		}


		// Render 2D 
		glm::mat4 cameraTransform;
		Camera* mainCamera = nullptr;  // used as a weakref
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto&& [entity, transform, camera] : view.each()) {
				if (camera.Primary) {
					cameraTransform = transform.Transform();
					mainCamera = &camera.Camera;
					break;
				}
			}
		}

		if (!mainCamera) return;

		Renderer2D::BeginScene(*mainCamera, cameraTransform);

		{
			auto group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
			for (auto&& [entity, spriteComponent, transform] : group.each()) {
				Renderer2D::DrawSprite(transform.Transform(), spriteComponent.Sprite);
			}
		}

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
		m_Registry.each([this, &callback](auto entity) {
			callback(Entity(entity, this));
		});
	}

	Entity Scene::PrimaryCameraEntity() {
		auto view = m_Registry.view<CameraComponent>();
		for (auto&&[entity, cameraComponent] : view.each()) {
			if (cameraComponent.Primary) {
				return Entity(entity, this);
			}
		}
		return Entity();
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component) {
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded(Entity entity, IDComponent& component) {

	}
	template void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component);

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
	void Scene::OnComponentAdded(Entity entity, CameraComponent& component) {
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
	template void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component);

	template<>
	void Scene::OnComponentAdded(Entity entity, NativeScriptComponent& component) {

	}
	template void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component);

	template<>
	void Scene::OnComponentAdded(Entity entity, RigidBody2DComponent& component) {

	}
	template void Scene::OnComponentAdded<RigidBody2DComponent>(Entity entity, RigidBody2DComponent& component);

	template<>
	void Scene::OnComponentAdded(Entity entity, BoxCollider2DComponent& component) {

	}
	template void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component);

}
