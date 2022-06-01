#pragma once

#include <entt.hpp>

#include "Pistachio/Core/Timestep.h"

#include "Pistachio/Scene/Components.h"


namespace Pistachio {

	class Entity;

	class Scene {
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string&name = std::string());
		void DestroyEntity(Entity entity);

		void OnUpdate(Timestep timestep);

		void OnViewportResize(unsigned int width, unsigned int height);

		void EachEntity(std::function<void(Entity)> callback);

		Entity PrimaryCameraEntity();

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		//template<>
		//void OnComponentAdded(Entity entity, TagComponent& component);

		//template<>
		//void OnComponentAdded(Entity entity, TransformComponent& component);

		//template<>
		//void OnComponentAdded(Entity entity, SpriteRendererComponent& component);

		//template<>
		//void OnComponentAdded(Entity entity, CameraComponent& component);

		//template<>
		//void OnComponentAdded(Entity entity, NativeScriptComponent& component);

	private:
		entt::registry m_Registry;
		unsigned int m_ViewportWidth = 9, m_ViewportHeight = 0;

		friend class Entity;
	};

}
