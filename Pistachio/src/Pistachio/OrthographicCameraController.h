#pragma once

#include "Pistachio/Renderer/OrthographicCamera.h"
#include "Pistachio/Core/Timestep.h"

#include "Pistachio/Events/EventListener.h"


namespace Pistachio {

	class OrthographicCameraController : public EventListener {
	public:
		OrthographicCameraController(unsigned int width, unsigned int height, bool rotation = false);

		void OnUpdate(Timestep timestep);

		OrthographicCamera& Camera() { return m_Camera; }
		const OrthographicCamera& Camera() const { return m_Camera; }

	private:
		bool OnWindowResize(WindowResizeEvent& event);

		bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& event);
		bool OnMouseMoved(MouseMovedEvent& event);
		bool OnMouseScrolled(MouseScrolledEvent& event);

	private:
		unsigned int m_Width, m_Height;
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;

		OrthographicCamera m_Camera;

		bool m_Rotation;
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;

		float m_CameraMoveSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;

		bool m_MouseLeftButtonHeld = false;
		std::pair<float, float> m_MousePressedPosition = {0.0f, 0.0f};
		std::pair<float, float> m_MousePressedCameraPosition = { 0.0f, 0.0f };
	};

}
