#pragma once

#include <filesystem>

#include "Pistachio/Renderer/Texture.h"

#include "Pistachio/Events/EventListener.h"


namespace Pistachio {

	class ContentBrowserPanel : public EventListener {
	public:
		ContentBrowserPanel(const std::string& directory);

		void OnImGuiRender();

	private:
		virtual bool OnMouseScrolled(MouseScrolledEvent& event) override;

	private:
		std::filesystem::path m_AssetsDirectory;
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;

		unsigned int m_ThumbnailSize = 128;
		unsigned int m_Padding = 16;
		bool m_IsHovered = false;
	};

}
