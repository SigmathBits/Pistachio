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
		template<typename T>
		void DrawComponentProperties(Entity entity, const std::string& label, std::function<void(T&)> drawFunction, bool removable = true) {
			if (entity.HasComponent<T>()) {
				//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 2.0f, 2.0f });

				bool open = ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);

				bool removeComponent = false;
				if (removable) {
					ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
					if (ImGui::Button("+", ImVec2{ 20.0f, 20.0f })) {
						ImGui::OpenPopup("Component Settings");
					}

					if (ImGui::BeginPopup("Component Settings")) {
						if (ImGui::MenuItem("Remove component")) {
							removeComponent = true;
						}

						ImGui::EndPopup();
					}
				}
				//ImGui::PopStyleVar();

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

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

	private:
		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};

}
