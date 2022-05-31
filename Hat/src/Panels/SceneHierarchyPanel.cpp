#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>

#include "Pistachio/Scene/Components.h"

// FIXME: Remove
#include "Pistachio/Renderer/Texture.h"


namespace Pistachio {

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
		: m_Context(scene) {
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene) {
		m_Context = scene;
	}

	void SceneHierarchyPanel::OnImGuiRender() {
		ImGui::Begin("Scene Hierarchy");

		m_Context->EachEntity([this](Entity entity) {
			DrawEntityNode(entity);
		});

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
			m_SelectedEntity = {};
		}

		// Right Click on blank panel area
		if (ImGui::BeginPopupContextWindow("Scene Context Menu", ImGuiMouseButton_Right, false)) {
			if (ImGui::MenuItem("Create Empty Entity")) {
				m_Context->CreateEntity("Empty Entity");
			}

			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity) {
		std::string& tag = entity.Component<TagComponent>().Tag;
		
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | (m_SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0);
		bool opened = ImGui::TreeNodeEx((void*)(size_t)(unsigned int)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked()) {
			m_SelectedEntity = entity;
		}

		bool deleteEntity = false;
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete Entity")) {
				deleteEntity = true;
			}

			ImGui::EndPopup();
		}

		if (opened) {
			ImGui::TreePop();
		}

		if (deleteEntity) {
			if (m_SelectedEntity == entity) {
				m_SelectedEntity = {};
			}
			m_Context->DestroyEntity(entity);
		}
	}

}
