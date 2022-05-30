#include "pstpch.h"

#include "Scene.h"

#include <glm/glm.hpp>

#include "Pistachio/Renderer/Renderer2D.h"

#include "Pistachio/Scene/Entity.h"
#include "Pistachio/Scene/Components.h"


namespace Pistachio {

	Scene::Scene() {

	}

	Scene::~Scene() {
	}

	Entity Scene::CreateEntity(const std::string& name /*= std::string()*/) {
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);
		return entity;
	}

	void Scene::OnUpdate(Timestep timestep) {
		// Render 2D 
		glm::mat4* cameraTransform = nullptr;
		Camera* mainCamera = nullptr;
		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto&& [entity, transform, camera] : view.each()) {
			if (camera.Primary) {
				cameraTransform = &transform.Transform;
				mainCamera = &camera.Camera;
				break;
			}
		}

		if (!mainCamera) return;

		Renderer2D::BeginScene(*mainCamera, *cameraTransform);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto&& [entity, transform, sprite] : group.each()) {
			Renderer2D::DrawQuad(transform, sprite.Colour);
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

}
