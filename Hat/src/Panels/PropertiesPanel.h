#pragma once

#include "Pistachio/Scene/Entity.h"


namespace Pistachio {

	class PropertiesPanel {
	public:
		PropertiesPanel();

		inline void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }

		void OnImGuiRender();

	private:
		void DrawComponents(Entity entity);
		static void DrawAddComponentPopup(Entity entity);

	private:
		Entity m_SelectedEntity;

		Ref<Texture2D> m_ImageIcon;
	};

}
