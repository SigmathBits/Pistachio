#pragma once

#include "Pistachio/Scene/Entity.h"


namespace Pistachio {

	class PropertiesPanel {
	public:
		inline void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }

		void OnImGuiRender();

	private:
		static void DrawComponents(Entity entity);
		static void DrawAddComponentPopup(Entity entity);

	private:
		Entity m_SelectedEntity;
	};

}
