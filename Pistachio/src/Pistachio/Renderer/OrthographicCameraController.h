#pragma once

#include "Pistachio/Core/Timestep.h"

#include "Pistachio/Events/EventListener.h"

#include "Pistachio/Renderer/OrthographicCamera.h"


namespace Pistachio {

	class OrthographicCameraController : public EventListener {
	public:
		OrthographicCameraController(unsigned int width, unsigned int height, bool rotation = false);

		void OnUpdate(Timestep timestep);

		OrthographicCamera& Camera() { return m_Camera; }
		const OrthographicCamera& Camera() const { return m_Camera; }

		void SetZoom(float zoom);

	private:
		bool OnWindowResize(WindowResizeEvent& event);

		bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& event);
		bool OnMouseMoved(MouseMovedEvent& event);
		bool OnMouseScrolled(MouseScrolledEvent& event);

		bool OnKeyPressed(KeyPressedEvent& event);

	private:
		unsigned int m_Width, m_Height;
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;

		OrthographicCamera m_Camera;

		bool m_Rotation;

		const float m_CameraAngleSpeed = 15.0f;

		bool m_CameraModePan = true;
		bool m_MouseLeftButtonHeld = false;
		glm::vec2 m_MousePressedPosition = { 0.0f, 0.0f };
		glm::vec3 m_MousePressedCameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_MousePressedRotation = 0.0f;
	};

}
