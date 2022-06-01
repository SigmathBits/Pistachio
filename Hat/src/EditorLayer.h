#pragma once

#include <Pistachio.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/PropertiesPanel.h"


namespace Pistachio {

	class EditorLayer : public Pistachio::Layer {
	public:
		EditorLayer();
		virtual ~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Pistachio::Timestep timestep) override;

		virtual void OnImGuiRender() override;

		bool OnEvent(Event& event) override;

	private:
		bool OnKeyPressed(KeyPressedEvent& event) override;

		void NewScene();
		void FileOpen();
		void FileSave();
		void FileSaveAs();

		void LoadSceneFile(std::string& filepath);
		void SaveSceneFile(std::string& filepath);
		void SetLastSave(std::string& filepath);

	private:
		OrthographicCameraController m_CameraController;

		Ref<Framebuffer> m_Framebuffer; 

		std::string m_Filepath;
		Ref<Scene> m_ActiveScene;

		Ref<Texture2D> m_PistachioTexture;
		Ref<Texture2D> m_RainbowDashTexture;
		Ref<Texture2D> m_CheckerboardTexture;

		glm::vec2 m_ViewportPosition{ 0.0f, 0.0f };
		glm::vec2 m_ViewportSize{ 1280, 720 };
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		bool m_ViewportIsDragging = false;

		int m_GizmoType = -1;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		PropertiesPanel m_PropertiesPanel;
	};

}
