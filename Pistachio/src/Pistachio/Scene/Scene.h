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
		Scene(const std::string& name = "Untitled");
		~Scene();

		static Ref<Scene> Copy(const Ref<Scene> other);

		Entity CreateEntity(const std::string&name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string&name = std::string());
		Entity DuplicateEntity(Entity entity);
		void DestroyEntity(Entity entity);

		inline const std::string& Name() const { return m_Name; }
		inline void SetName(const std::string& name) { m_Name = name; }

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateEditor(Timestep timestep, EditorCamera& camera);
		void OnUpdateRuntime(Timestep timestep);
		void OnUpdateSimulation(Timestep timestep, EditorCamera& camera);

		void OnViewportResize(unsigned int width, unsigned int height);

		Entity PrimaryCameraEntity();

		void EachEntity(std::function<void(Entity)> callback);

		template<typename... C>
		void EachEntityWith(std::function<void(Entity)> callback) {
			auto view = m_Registry.view<C...>();
			view.each([&callback, this](auto enttID, C&... components) {
				callback(Entity{ enttID, this });
			});
		}

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void Init2DPhysics();
		void Destroy2DPhysics();
		void Update2DPhysics(Timestep timestep);

		void RenderScene();

	private:
		std::string m_Name;

		entt::registry m_Registry;
		unsigned int m_ViewportWidth = 9, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;
		std::unordered_map<UUID, b2Body*> m_RuntimeBodies;

		friend class Entity;
	};

}
