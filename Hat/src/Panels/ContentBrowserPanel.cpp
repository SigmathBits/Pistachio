#include "pstpch.h"

#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

#include "Pistachio/Core/Application.h"
#include "Pistachio/Core/Input.h"


namespace Pistachio {

	ContentBrowserPanel::ContentBrowserPanel(const std::string& directory) 
		: EventListener({ EventType::MouseScrolled }), m_AssetsDirectory(directory), m_CurrentDirectory(directory) {
		m_DirectoryIcon = Texture2D::Create("resources/icons/content-browser/folder.png");
		m_FileIcon = Texture2D::Create("resources/icons/content-browser/file.png");
	}

	void ContentBrowserPanel::OnImGuiRender() {
		ImGui::Begin("Content Browser");

		// Allow events Pistachio Event's to pass through ImGui to us if the content browser is hovered and the user is holding control
		// We need events to later capture MouseScolledEvents to adjust the icon size (when control is held)
		m_IsHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
		bool blockMouseEvents = Application::Instance().BaseImGuiLayer()->MouseEventsBlocked() 
			&& !(m_IsHovered && Input::IsKeyPressed(PST_KEY_LEFT_CONTROL));
		Application::Instance().BaseImGuiLayer()->AllowMouseEventBlocking(blockMouseEvents);

		ImGui::BeginDisabled(m_CurrentDirectory == m_AssetsDirectory);
		if (ImGui::Button("<-")) {
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}
		ImGui::EndDisabled();

		ImGui::SameLine();

		ImGui::BeginDisabled();
		auto directoryString = m_CurrentDirectory.string() + "\\";
		std::replace(directoryString.begin(), directoryString.end(), '\\', '/');

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strncpy_s(buffer, directoryString.c_str(), directoryString.size() + 1);

		ImGui::InputText("##Directory", buffer, sizeof(buffer));
		ImGui::EndDisabled();
		
		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		float cellSize = (float)(m_ThumbnailSize + m_Padding);
		int columnCount = std::max((int)(panelSize.x / cellSize), 1);

		ImGui::BeginChild("Files", panelSize);

		ImGui::Columns(columnCount, nullptr, false);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
			const auto& path = directoryEntry.path();
			std::string filename = path.filename().string();

			ImGui::PushID(path.string().c_str());

			auto icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::ImageButton((ImTextureID)icon->RendererID(), { (float)m_ThumbnailSize, (float)m_ThumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				std::string itemPath = path.string();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath.c_str(), itemPath.size() + 1);
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (directoryEntry.is_directory()) {
					m_CurrentDirectory = path;
				}
			}

			ImGui::BeginHorizontal("Name", { cellSize, 0.0f });
			ImGui::Spring(1);
			ImGui::TextWrapped(filename.c_str());
			ImGui::Spring(1);
			ImGui::EndHorizontal();

			ImGui::PopID();

			ImGui::NextColumn();
		}
		ImGui::PopStyleColor();

		ImGui::Columns(1);

		ImGui::EndChild();

		ImGui::End();
	}

	bool ContentBrowserPanel::OnMouseScrolled(MouseScrolledEvent& event) {
		if (m_IsHovered && Input::IsKeyPressed(PST_KEY_LEFT_CONTROL)) {
			m_ThumbnailSize = std::clamp((int)std::round((float)m_ThumbnailSize * std::pow(2.0f, 0.25f * event.YOffset())), 64, 512);
			m_Padding = std::max((int)std::round(16.0f * (1 - ((float)m_ThumbnailSize - 64.0f) / (128.0f - 64.0f)) + 16.0f), 16);
			return true;
		}
		return false;
	}

}


