#pragma once

#include <Pistachio.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Panels/ContentBrowserPanel.h"


namespace Pistachio {

	class EditorLayer : public Pistachio::Layer {
	public:
		EditorLayer();
		virtual ~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Pistachio::Timestep timestep) override;

		virtual void OnOverlayRender();

		virtual void OnImGuiRender() override;

		void UIToolbar();

	private:
		void OnScenePlay();
		void OnSceneStop();

		void OnSceneSimulationPlay();

		bool OnEvent(Event& event) override;
		bool OnEventAfter(Event& event) override;

		bool OnKeyPressed(KeyPressedEvent& event) override;
		bool OnMouseButtonPressed(MouseButtonPressedEvent& event) override;
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& event) override;

		void FileNew();
		void FileOpen();
		void FileSave();
		void FileSaveAs();

		void LoadSceneFile(const std::filesystem::path& filepath);
		void SaveSceneFile(const std::filesystem::path& filepath);
		void SetLastSave(const std::filesystem::path& filepath);

	private:
		void ChangeSceneTo(Ref<Scene> scene);
		void ChangeActiveSceneTo(Ref<Scene> scene);

		void SetSelectedEntity(Entity entity);
		void SetWindowTitle(const std::string& title, bool unsavedChanges = false);

	private:
		EditorCamera m_EditorCamera;

		Ref<Framebuffer> m_Framebuffer; 

		std::filesystem::path m_Filepath;
		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		Ref<Scene> m_RuntimeScene;

		glm::vec2 m_ViewportPosition{ 0.0f, 0.0f };
		glm::vec2 m_ViewportSize{ 1280, 720 };
		glm::vec2 m_ViewportBounds[2];

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		bool m_ViewportIsDragging = false;

		int m_GizmoType = -1;
		bool m_ShowPhysicsColliders = true;
		
		Entity m_HoveredEntity;

		enum class SceneState {
			Edit = 0, Play,
			Simulate,
		};

		SceneState m_SceneState = SceneState::Edit;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		PropertiesPanel m_PropertiesPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		// Editor Resources
		Ref<Texture2D> m_PlayIcon;
		Ref<Texture2D> m_SimulateIcon;
		Ref<Texture2D> m_StopIcon;
	};

}
