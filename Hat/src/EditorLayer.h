#pragma once

#include <Pistachio.h>

#include "Panels/SceneHierarchyPanel.h"


namespace Pistachio {

	class EditorLayer : public Pistachio::Layer {
	public:
		EditorLayer();
		virtual ~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Pistachio::Timestep timestep) override;

		virtual void OnImGuiRender() override;

		bool OnEvent(Pistachio::Event& event) override;

	private:
		OrthographicCameraController m_CameraController;

		Ref<Framebuffer> m_Framebuffer; 

		Ref<Scene> m_ActiveScene;

		Ref<Texture2D> m_PistachioTexture;
		Ref<Texture2D> m_RainbowDashTexture;
		Ref<Texture2D> m_CheckerboardTexture;

		glm::vec2 m_ViewportPosition{ 0.0f, 0.0f };
		glm::vec2 m_ViewportSize{ 1280, 720 };
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		bool m_ViewportIsDragging = false;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};

}
