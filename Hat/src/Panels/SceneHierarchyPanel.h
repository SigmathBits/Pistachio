#pragma once

#include "Pistachio/Core/Core.h"

#include "Pistachio/Scene/Scene.h"
#include "Pistachio/Scene/Entity.h"


namespace Pistachio {

	class SceneHierarchyPanel {
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		inline Entity SelectedEntity() const { return m_SelectedEntity; }

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();

	private:
		void DrawEntityNode(Entity entity);

	private:
		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};

}
