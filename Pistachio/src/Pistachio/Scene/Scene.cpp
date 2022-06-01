#include "pstpch.h"

#include "Scene.h"

#include <glm/glm.hpp>

#include "Pistachio/Renderer/Renderer2D.h"
#include "Pistachio/Renderer/EditorCamera.h"

#include "Pistachio/Scene/Entity.h"
#include "Pistachio/Scene/ScriptableEntity.h"


namespace Pistachio {

	Scene::Scene() {

	}

	Scene::~Scene() {
	}

	Entity Scene::CreateEntity(const std::string& name /*= std::string()*/) {
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name.empty() ? "Unnamed Entity" : name);
		return entity;
	}

	void Scene::DestroyEntity(Entity entity) {
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateEditor(Timestep timestep, EditorCamera& camera) {
		Renderer2D::BeginScene(camera);

		{
			auto group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
			for (auto&& [entity, spriteComponent, transform] : group.each()) {
				Renderer2D::DrawQuad(transform.Transform(), spriteComponent.Sprite);
			}
		}

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateRuntime(Timestep timestep) {
		// Update Scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& script) {
				// TODO: Move to Scene::OnScenePlay
				if (!script.ScriptInstance) {
					script.ScriptInstance = script.CreateScriptInstance();
					script.ScriptInstance->m_Entity = Entity(entity, this);
					script.ScriptInstance->OnCreate();
				}

				script.ScriptInstance->OnUpdate(timestep);

				// TODO: Call OnDestroy in a Scene::OnSceneStop
			});
		}

		// Render 2D 
		glm::mat4 cameraTransform;
		Camera* mainCamera = nullptr;
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
				Renderer2D::DrawQuad(transform.Transform(), spriteComponent.Sprite);
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

}
