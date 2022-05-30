#pragma once

#include "entt.hpp"

#include "Pistachio/Core/Timestep.h"


namespace Pistachio {

	class Entity;

	class Scene {
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string&name = std::string());

		void OnUpdate(Timestep timestep);

		void OnViewportResize(unsigned int width, unsigned int height);

	private:
		entt::registry m_Registry;
		unsigned int m_ViewportWidth = 9, m_ViewportHeight = 0;

		friend class Entity;
	};

}
