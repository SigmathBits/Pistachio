#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>

#include "Pistachio/Core/Input.h"

#include "Pistachio/Scene/Components.h"


namespace Pistachio {

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
		: m_Context(scene) {
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene) {
		m_Context = scene;
		m_SelectedEntity = {};
		m_EditingEntity = {};
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity) {
		if (m_SelectedEntity != entity) {
			m_SelectedEntity = entity;
			m_EditingEntity = {};
		}
	}

	void SceneHierarchyPanel::OnImGuiRender() {
		ImGui::Begin("Scene Hierarchy");

		m_Context->EachEntity([this](Entity entity) {
			DrawEntityNode(entity);
		});

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
			SetSelectedEntity({});
		}

		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) || ImGui::IsKeyDown(ImGuiKey_F2)) {
			m_EditingEntity = m_SelectedEntity;
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
		
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | (m_SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0);
		if (m_EditingEntity != entity) {
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		}

		bool opened = ImGui::TreeNodeEx((void*)(size_t)(unsigned int)entity, flags, "");

		if (ImGui::IsItemClicked()) {
			SetSelectedEntity(entity);
		}

		// Context menu logic
		bool deleteEntity = false;
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Rename Entity")) {
				m_EditingEntity = entity;
			}

			if (ImGui::MenuItem("Delete Entity")) {
				deleteEntity = true;
			}

			if (ImGui::MenuItem("Duplicate Entity")) {
				m_Context->DuplicateEntity(entity);
			}

			ImGui::EndPopup();
		}

		// Reordering logic
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			ImGui::SetDragDropPayload("SCENE_HIEREARCHY_ENTITY", &entity, sizeof(entity));

			ImGui::Text(entity.Name().c_str());

			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIEREARCHY_ENTITY")) {
				Entity& droppedEntity = *(Entity*)payload->Data;
				m_Context->MoveEntityToEntityPosition(droppedEntity, entity);
			}
			ImGui::EndDragDropTarget();
		}

		// Editing Tag logic
		ImGui::SameLine();
		if (m_EditingEntity == entity) {
			char buffer[256];
			strncpy(buffer, tag.c_str(), tag.size() + 1);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 0.0f));
			if (ImGui::InputText("###Rename", buffer, sizeof(buffer), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
				tag = buffer;
				m_EditingEntity = {};
			}
			ImGui::PopStyleVar();

			ImGui::SetKeyboardFocusHere(-1);

			if (ImGui::IsKeyDown(ImGuiKey_Escape) || ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered()) {
				m_EditingEntity = {};
			}
		} else {
			ImGui::Text(tag.c_str());
		}

		if (opened) {
			ImGui::TreePop();
		}

		// Deletion logic
		if (deleteEntity) {
			if (m_SelectedEntity == entity) {
				SetSelectedEntity({});
			}
			m_Context->DestroyEntity(entity);
		}
	}

}
