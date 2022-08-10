#include "pstpch.h"

#include "PropertiesPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include "Pistachio/Utils/Utils.h"
#include "Pistachio/Utils/ImGuiUtils.h"


namespace Pistachio {

	template<typename C>
	static void DrawComponentProperties(Entity entity, const std::string& label, std::function<void(C&)> drawFunction, bool removable = true);

	template<typename C>
	static bool DrawAddComponentPopupItem(Entity entity, const std::string& name);

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
			constexpr char* noSelectionText = "No Entity Selected";

			float windowWidth = ImGui::GetWindowSize().x;
			float textWidth = ImGui::CalcTextSize(noSelectionText).x;

			ImGui::SetCursorPosX(0.5f * (windowWidth - textWidth));

			Utils::ImGuiPushFontStyle(Utils::ITALICS);
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
			// FIXME: Could be issues if tag exceeds 255 characters
			strncpy(buffer, tag.c_str(), tag.size() + 1);

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
				Utils::ImGuiImageButton(m_ImageIcon, { 128.0f, 128.0f });
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
			ImGui::DragFloat2("Offset", glm::value_ptr(boxColliderComponent.Offset), 0.05f);
			ImGui::DragFloat2("Size", glm::value_ptr(boxColliderComponent.Size), 0.05f);

			ImGui::SliderFloat("Density", &boxColliderComponent.Density, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("Friction", &boxColliderComponent.Friction, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("Restitution", &boxColliderComponent.Restitution, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragFloat("Restitution Threshold", &boxColliderComponent.RestitutionThreshold, 0.01f, 0.0f, +FLT_MAX, nullptr, ImGuiSliderFlags_AlwaysClamp);
		});

		DrawComponentProperties<CircleCollider2DComponent>(entity, "Circle Collider 2D", [this](auto& circleColliderComponent) {
			ImGui::DragFloat2("Offest", glm::value_ptr(circleColliderComponent.Offset), 0.05f);
			ImGui::DragFloat("Radius", &circleColliderComponent.Radius, 0.05f, 0.0f, +FLT_MAX, nullptr, ImGuiSliderFlags_AlwaysClamp);

			ImGui::SliderFloat("Density", &circleColliderComponent.Density, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("Friction", &circleColliderComponent.Friction, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("Restitution", &circleColliderComponent.Restitution, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragFloat("Restitution Threshold", &circleColliderComponent.RestitutionThreshold, 0.01f, 0.0f, +FLT_MAX, nullptr, ImGuiSliderFlags_AlwaysClamp);
		});
	}

	void PropertiesPanel::DrawAddComponentPopup(Entity entity) {
		if (ImGui::BeginPopup("Add Component Popup")) {
			bool canAddComponents = false;

			canAddComponents |= DrawAddComponentPopupItem<SpriteRendererComponent>(entity, "Sprite Renderer");
			canAddComponents |= DrawAddComponentPopupItem<CircleRendererComponent>(entity, "Circle Renderer");

			canAddComponents |= DrawAddComponentPopupItem<CameraComponent>(entity, "Camera");

			canAddComponents |= DrawAddComponentPopupItem<RigidBody2DComponent>(entity, "Rigid Body 2D");
			canAddComponents |= DrawAddComponentPopupItem<BoxCollider2DComponent>(entity, "Box Collider 2D");
			canAddComponents |= DrawAddComponentPopupItem<CircleCollider2DComponent>(entity, "Circle Collider 2D");

			if (!canAddComponents) {
				Utils::ImGuiPushFontStyle(Utils::ITALICS);
				ImGui::TextDisabled("No Available Components");
				ImGui::PopFont();
			}

			ImGui::EndPopup();
		}
	}

	template<typename C>
	static void DrawComponentProperties(Entity entity, const std::string& label, std::function<void(C&)> drawFunction, bool removable /*= true*/) {
		if (!entity.HasComponent<C>()) return;

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

			if (Utils::ImGuiButton("+", { lineHeight, lineHeight }, Utils::GREEN, Utils::BOLD)) {
				ImGui::OpenPopup("Component Settings");
			}

			if (ImGui::BeginPopup("Component Settings")) {
				if (ImGui::MenuItem("Remove component")) {
					removeComponent = true;
				}

				ImGui::EndPopup();
			}
		}
		ImGui::PopStyleVar();  // Pop Frame Padding

		if (open) {
			C& component = entity.Component<C>();
			drawFunction(component);
			ImGui::TreePop();
		}

		if (removeComponent) {
			entity.RemoveComponent<C>();
		}
	}

	/// <returns>Whether the add component popup item was drawn</returns>
	template<typename C>
	static bool DrawAddComponentPopupItem(Entity entity, const std::string& name) {
		if (entity.HasComponent<C>()) return false;

		if (ImGui::MenuItem(name.c_str())) {
			entity.AddComponent<C>();
			ImGui::CloseCurrentPopup();
		}

		return true;
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
		glm::vec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		bool changed = false;

		// X control
		if (Utils::ImGuiButton("X", buttonSize, Utils::RED, Utils::BOLD)) {
			values.x = resetValue;
			changed = true;
		}

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		// Y Control
		if (Utils::ImGuiButton("Y", buttonSize, Utils::GREEN, Utils::BOLD)) {
			values.y = resetValue;
			changed = true;
		}

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		// Z Control
		if (Utils::ImGuiButton("Z", buttonSize, Utils::BLUE, Utils::BOLD)) {
			values.z = resetValue;
			changed = true;
		}

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();  // Pop Item Spacing
		ImGui::Columns(1);

		ImGui::PopID();

		return changed;
	}

}
