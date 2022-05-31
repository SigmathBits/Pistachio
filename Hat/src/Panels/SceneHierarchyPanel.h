#pragma once

#include "Pistachio/Core/Core.h"

#include "Pistachio/Scene/Scene.h"
#include "Pistachio/Scene/Entity.h"


namespace Pistachio {

	class SceneHierarchyPanel {
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

	private:
		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};

}
