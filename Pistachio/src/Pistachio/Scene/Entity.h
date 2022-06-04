#pragma once

#include <entt.hpp>

#include "Pistachio/Core/Core.h"
#include "Pistachio/Core/Log.h"
#include "Pistachio/Core/UUID.h"

#include "Pistachio/Scene/Scene.h"
#include "Pistachio/Scene/Components.h"


namespace Pistachio {

	class Entity {
	public:
		Entity() = default;
		Entity(entt::entity entityHandle, Scene* scene);
		Entity(const Entity& other) = default;

		UUID UUID() { return Component<UUIDComponent>().UUID; }
		const std::string& Name() { return Component<TagComponent>().Tag; }

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) {
			PST_CORE_ASSERT(!HasComponent<T>(), "Entity already has component");

			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);

			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args) {
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& Component() {
			PST_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");

			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent() {
			PST_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");

			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent() {
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		bool operator==(const Entity& other) const { 
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}
		
		bool operator!=(const Entity& other) const { 
			return m_EntityHandle != other.m_EntityHandle || m_Scene != other.m_Scene;
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator unsigned int() const { return (unsigned int)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }

	private:
		entt::entity m_EntityHandle = entt::null;
		Scene* m_Scene = nullptr;  // used as a weakref
	};

}
