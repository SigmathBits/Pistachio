#pragma once

#include <Pistachio.h>


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

		glm::vec2 m_ViewportPosition;
		glm::vec2 m_ViewportSize{ 1280, 720 };
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		bool m_ViewportIsDragging = false;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCameraEntity;

		bool m_UsePrimaryCamera = true;

		Ref<Texture2D> m_PistachioTexture;
		Ref<Texture2D> m_RainbowDashTexture;
		Ref<Texture2D> m_CheckerboardTexture;

		RotatedTransform m_RotatedTransform{ { 0.0f, 0.0f, -0.51f }, 0.0f };
		float m_Angle = 0.0f;
		glm::vec4 m_Colour{ 0.486f, 0.686f, 0.255f, 1.0f };
	};

}
