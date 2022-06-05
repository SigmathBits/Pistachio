#include "pstpch.h"

#include "PropertiesPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include "Pistachio/Utils/Utils.h"


namespace Pistachio {

	template<typename T>
	static void DrawComponentProperties(Entity entity, const std::string& label, std::function<void(T&)> drawFunction, bool removable = true);

	static bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);


	PropertiesPanel::PropertiesPanel() {
		m_ImageIcon = Texture2D::Create("resources/icons/content-browser/image.png", 3);
	}

	void PropertiesPanel::OnImGuiRender() {
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

	void PropertiesPanel::DrawComponents(Entity entity) {
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
					if (ImGui::SliderFloat("Vertical FOV", &perspectiveVerticalFOV, 0.0f, 180.0f, "%.1f deg", ImGuiSliderFlags_AlwaysClamp)) {
						camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFOV));
					}

					float perspectiveNearClip = camera.PerspectiveNearClip();
					if (ImGui::DragFloat("Near Clip", &perspectiveNearClip, 0.1f, -FLT_MAX, camera.PerspectiveFarClip(), nullptr, ImGuiSliderFlags_AlwaysClamp)) {
						camera.SetPerspectiveNearClip(perspectiveNearClip);
					}

					float perspectiveFarClip = camera.PerspectiveFarClip();
					if (ImGui::DragFloat("Far Clip", &perspectiveFarClip, 0.1f, camera.PerspectiveNearClip(), +FLT_MAX, nullptr, ImGuiSliderFlags_AlwaysClamp)) {
						camera.SetPerspectiveFarClip(perspectiveFarClip);
					}

					break;
				}

				case SceneCamera::ProjectionType::Orthographic:
				{
					ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);

					float orthoSize = camera.OrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize, 0.1f, 0.0f, +FLT_MAX, nullptr, ImGuiSliderFlags_AlwaysClamp)) {
						camera.SetOrthographicSize(orthoSize);
					}

					float orthoNearClip = camera.OrthographicNearClip();
					if (ImGui::DragFloat("Near Clip", &orthoNearClip, 0.1f, -FLT_MAX, camera.OrthographicFarClip(), nullptr, ImGuiSliderFlags_AlwaysClamp)) {
						camera.SetOrthographicNearClip(orthoNearClip);
					}

					float orthoFarClip = camera.OrthographicFarClip();
					if (ImGui::DragFloat("Far Clip", &orthoFarClip, 0.1f, camera.OrthographicNearClip(), +FLT_MAX, nullptr, ImGuiSliderFlags_AlwaysClamp)) {
						camera.SetOrthographicFarClip(orthoFarClip);
					}

					break;
				}
			}
		});

		DrawComponentProperties<SpriteRendererComponent>(entity, "Sprite Renderer", [this](auto& spriteComponent) {
			if (spriteComponent.Sprite.SubTexture->Texture()) {
				ImGui::ImageButton((ImTextureID)m_ImageIcon->RendererID(), { 128.0f, 128.0f }, { 0, 1 }, { 1, 0 }, 0);
				if (ImGui::BeginPopupContextItem(nullptr, ImGuiMouseButton_Right)) {
					if (ImGui::MenuItem("Remove Texture")) {
						spriteComponent.Sprite.SubTexture = CreateRef<SubTexture2D>(nullptr);
					}
					ImGui::EndPopup();
				}
			} else {
				ImGui::Button("Drag & Drop\n   Texture  ", { 128.0f, 128.0f });
			}

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
					std::string path = (const char*)payload->Data;

					if (Utils::EndsWith(path, ".png")) {
						spriteComponent.Sprite.SubTexture = CreateRef<SubTexture2D>(Texture2D::Create(path));
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::ColorEdit4("Colour", glm::value_ptr(spriteComponent.Sprite.TintColour));
			ImGui::DragFloat("Tiling Factor", &spriteComponent.Sprite.TilingScale, 0.1f, 0.0f, 10.0f);
		});

		DrawComponentProperties<CircleRendererComponent>(entity, "Circle Renderer", [this](auto& circleComponent) {
			ImGui::ColorEdit4("Colour", glm::value_ptr(circleComponent.Colour));
			ImGui::SliderFloat("Thickness", &circleComponent.Thickness, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("Blur", &circleComponent.Blur, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
		});

		DrawComponentProperties<RigidBody2DComponent>(entity, "Rigid Body 2D", [this](auto& rigidBodyComponent) {
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)rigidBodyComponent.Type];

			if (ImGui::BeginCombo("Projection", currentBodyTypeString)) {

				for (size_t i = 0; i < 3; i++) {
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected)) {
						currentBodyTypeString = bodyTypeStrings[i];
						rigidBodyComponent.Type = (RigidBody2DComponent::BodyType)i;
					}

					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::Checkbox("Fixed Rotation", &rigidBodyComponent.FixedRotation);
		});

		DrawComponentProperties<BoxCollider2DComponent>(entity, "Box Collider 2D", [this](auto& boxColliderComponent) {
			ImGui::DragFloat2("Offest", glm::value_ptr(boxColliderComponent.Offset));
			ImGui::DragFloat2("Size", glm::value_ptr(boxColliderComponent.Size));

			ImGui::SliderFloat("Density", &boxColliderComponent.Density, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("Friction", &boxColliderComponent.Friction, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("Restitution", &boxColliderComponent.Restitution, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragFloat("Restitution Threshold", &boxColliderComponent.RestitutionThreshold, 0.01f, 0.0f, +FLT_MAX, nullptr, ImGuiSliderFlags_AlwaysClamp);
		});
	}

	void PropertiesPanel::DrawAddComponentPopup(Entity entity) {
		if (ImGui::BeginPopup("Add Component Popup")) {
			bool isEmpty = true;

			if (!entity.HasComponent<SpriteRendererComponent>()) {
				isEmpty = false;
				if (ImGui::MenuItem("Sprite Renderer")) {
					entity.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!entity.HasComponent<CircleRendererComponent>()) {
				isEmpty = false;
				if (ImGui::MenuItem("Circle Renderer")) {
					entity.AddComponent<CircleRendererComponent>();
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

			if (!entity.HasComponent<RigidBody2DComponent>()) {
				isEmpty = false;
				if (ImGui::MenuItem("Rigid Body 2D")) {
					entity.AddComponent<RigidBody2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!entity.HasComponent<BoxCollider2DComponent>()) {
				isEmpty = false;
				if (ImGui::MenuItem("Box Collider 2D")) {
					entity.AddComponent<BoxCollider2DComponent>();
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
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.83f, 0.36f, 0.27f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.97f, 0.54f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.93f, 0.33f, 0.3f, 1.0f });
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
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.4f, 0.6f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.75f, 0.85f, 0.20f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.486f, 0.686f, 0.255f, 1.0f });
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
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.23f, 0.62f, 0.68f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.4f, 0.78f, 0.88f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.15f, 0.71f, 0.82f, 1.0f });
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

}
