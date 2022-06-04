#pragma once

#include <entt.hpp>

#include "Pistachio/Core/Timestep.h"

#include "Pistachio/Renderer/EditorCamera.h"

#include "Pistachio/Scene/Components.h"


class b2World;
class b2Body;


namespace Pistachio {

	class Entity;

	class Scene {
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string&name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string&name = std::string());
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateEditor(Timestep timestep, EditorCamera& camera);
		void OnUpdateRuntime(Timestep timestep);

		void OnViewportResize(unsigned int width, unsigned int height);

		void EachEntity(std::function<void(Entity)> callback);

		Entity PrimaryCameraEntity();

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

	private:
		entt::registry m_Registry;
		unsigned int m_ViewportWidth = 9, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;
		std::unordered_map<UUID, b2Body*> m_RuntimeBodies;

		friend class Entity;
	};

}
