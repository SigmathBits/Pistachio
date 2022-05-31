#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include "Pistachio/Scene/Components.h"

// FIXME: Remove
#include "Pistachio/Renderer/Texture.h"


namespace Pistachio {

	static void DrawAddComponentPopup(Entity entity);

	static bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

	template<typename T>
	static void DrawComponentProperties(Entity entity, const std::string& label, std::function<void(T&)> drawFunction, bool removable = true);


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


		ImGui::Begin("Properties");

		if (m_SelectedEntity) {
			// Right click on blank panel area
			if (ImGui::BeginPopupContextWindow("Properties Context Menu", ImGuiMouseButton_Right, false)) {
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();

				ImGui::OpenPopup("Add Component Popup");
			}

			DrawComponents(m_SelectedEntity);

			DrawAddComponentPopup(m_SelectedEntity);
		} else {
			ImGuiIO& io = ImGui::GetIO();
			auto italicFont = io.Fonts->Fonts[1];  // 1 is italics
			
			constexpr char* noSelectionText = "No Entity Selected";

			float windowWidth = ImGui::GetWindowSize().x;
			float textWidth = ImGui::CalcTextSize(noSelectionText).x;

			ImGui::SetCursorPosX(0.5f * (windowWidth - textWidth));

			ImGui::PushFont(italicFont);
			ImGui::TextDisabled(noSelectionText);
			ImGui::PopFont();
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

	void SceneHierarchyPanel::DrawComponents(Entity entity) {
		if (entity.HasComponent<TagComponent>()) {
			std::string& tag = entity.Component<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, tag.c_str(), tag.size() + 1);

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::Button("Add Component")) {
			ImGui::OpenPopup("Add Component Popup");
		}
		ImGui::PopItemWidth();

		DrawComponentProperties<TransformComponent>(entity, "Transform", [](auto& transformComponent) {
			DrawVec3Control("Translation", transformComponent.Translation);
			ImGui::Spacing();

			glm::vec3 rotationDegrees = glm::degrees(transformComponent.Rotation);
			if (DrawVec3Control("Rotation", rotationDegrees)) {
				transformComponent.Rotation = glm::radians(rotationDegrees);
			}
			ImGui::Spacing();

			DrawVec3Control("Scale", transformComponent.Scale, 1.0f);
			ImGui::Spacing();
		}, false);

		DrawComponentProperties<CameraComponent>(entity, "Camera", [](auto& cameraComponent) {
			auto& camera = cameraComponent.Camera;

			ImGui::Checkbox("Primary", &cameraComponent.Primary);

			const char* projectionTypeStrings[2] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(size_t)cameraComponent.Camera.CurrentProjectionType()];

			if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {

				for (size_t i = 0; i < 2; i++) {
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			switch (camera.CurrentProjectionType()) {
				case SceneCamera::ProjectionType::Perspective:
				{
					float perspectiveVerticalFOV = glm::degrees(camera.PerspectiveVerticalFOV());
					if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFOV, 0.1f, 0.0f, 180.0f)) {
						camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFOV));
					}

					float perspectiveNearClip = camera.PerspectiveNearClip();
					if (ImGui::DragFloat("Near Clip", &perspectiveNearClip, 0.1f, -FLT_MAX, camera.PerspectiveFarClip())) {
						camera.SetPerspectiveNearClip(perspectiveNearClip);
					}

					float perspectiveFarClip = camera.PerspectiveFarClip();
					if (ImGui::DragFloat("Far Clip", &perspectiveFarClip, 0.1f, camera.PerspectiveNearClip(), +FLT_MAX)) {
						camera.SetPerspectiveFarClip(perspectiveFarClip);
					}

					break;
				}

				case SceneCamera::ProjectionType::Orthographic:
				{
					ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);

					float orthoSize = camera.OrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize, 0.1f, 0.0f, +FLT_MAX)) {
						camera.SetOrthographicSize(orthoSize);
					}

					float orthoNearClip = camera.OrthographicNearClip();
					if (ImGui::DragFloat("Near Clip", &orthoNearClip, 0.1f, -FLT_MAX, camera.OrthographicFarClip())) {
						camera.SetOrthographicNearClip(orthoNearClip);
					}

					float orthoFarClip = camera.OrthographicFarClip();
					if (ImGui::DragFloat("Far Clip", &orthoFarClip, 0.1f, camera.OrthographicNearClip(), +FLT_MAX)) {
						camera.SetOrthographicFarClip(orthoFarClip);
					}

					break;
				}
			}
		});

		DrawComponentProperties<SpriteRendererComponent>(entity, "Sprite Renderer", [](auto& spriteComponent) {
			ImGui::ColorEdit4("Colour", glm::value_ptr(spriteComponent.Sprite.TintColour));
		});
	}

	static void DrawAddComponentPopup(Entity entity) {
		if (ImGui::BeginPopup("Add Component Popup")) {
			bool isEmpty = true;

			if (!entity.HasComponent<SpriteRendererComponent>()) {
				isEmpty = false;
				if (ImGui::MenuItem("Sprite Renderer")) {
					// Default White Texture
					// FIXME: This is a workaround, shouldn't be making textures here
					static auto whiteTexture = Texture2D::Create(1, 1);
					unsigned int whiteTextureData = 0xFFFFFFFF;
					whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

					entity.AddComponent<SpriteRendererComponent>(whiteTexture);
					ImGui::CloseCurrentPopup();
				}
			}

			if (!entity.HasComponent<CameraComponent>()) {
				isEmpty = false;
				if (ImGui::MenuItem("Camera")) {
					entity.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);  // 1 is italics
			if (isEmpty) {
				ImGui::TextDisabled("No Available Components");
			}
			ImGui::PopFont();

			ImGui::EndPopup();
		}
	}

	static bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue /*= 0.0f*/, float columnWidth /*= 100.0f*/) {
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];  // 0 is bold

		ImGui::PushID(label.c_str());

		ImGui::Columns(2, label.c_str(), false);
		ImGui::SetColumnWidth(0, columnWidth);

		ImGui::Text(label.c_str());

		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 0.0f });
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

		// How ImGui calculates line height
		float lineHeight = GImGui->FontSize + 2.0f * GImGui->Style.FramePadding.y;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		bool changed = false;

		// X control
		ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.83f, 0.36f, 0.27f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.97f, 0.54f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.93f, 0.33f, 0.3f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize)) {
			values.x = resetValue;
			changed = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();

		ImGui::SameLine();
		
		// Y Control
		ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.4f, 0.6f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.75f, 0.85f, 0.20f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.486f, 0.686f, 0.255f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize)) {
			values.y = resetValue;
			changed = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();

		ImGui::SameLine();
		
		// Z Control
		ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.23f, 0.62f, 0.68f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.4f, 0.78f, 0.88f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.15f, 0.71f, 0.82f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize)) {
			values.z = resetValue;
			changed = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();

		return changed;
	}

	template<typename T>
	static void DrawComponentProperties(Entity entity, const std::string& label, std::function<void(T&)> drawFunction, bool removable /*= true*/) {
		if (!entity.HasComponent<T>()) return;

		ImGuiTreeNodeFlags flags = 
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.0f, 4.0f });
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx(label.c_str(), flags);

		bool removeComponent = false;
		if (removable) {
			float lineHeight = ImGui::GetFrameHeight();

			ImGui::SameLine(contentRegionAvailable.x - 0.5f * lineHeight);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.4f, 0.6f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.75f, 0.85f, 0.20f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.486f, 0.686f, 0.255f, 1.0f });
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);  // 0 is bold
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) {
				ImGui::OpenPopup("Component Settings");
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			if (ImGui::BeginPopup("Component Settings")) {
				if (ImGui::MenuItem("Remove component")) {
					removeComponent = true;
				}

				ImGui::EndPopup();
			}
		}
		ImGui::PopStyleVar();

		if (open) {
			T& component = entity.Component<T>();
			drawFunction(component);
			ImGui::TreePop();
		}

		if (removeComponent) {
			entity.RemoveComponent<T>();
		}
	}

}
