#pragma once

#include "Pistachio/Core/Timestep.h"

#include "Pistachio/Events/EventListener.h"

#include "Pistachio/Renderer/OrthographicCamera.h"


namespace Pistachio {

	enum class OrthographicCameraControllerMoveMode : int {
		None = -1,
		Pan = 0,
		Rotate = 1,
	};

	class OrthographicCameraController : public EventListener {
	public:
		OrthographicCameraController(unsigned int width, unsigned int height, bool allowRotation = false);

		OrthographicCamera& Camera() { return m_Camera; }
		const OrthographicCamera& Camera() const { return m_Camera; }

		void Resize(unsigned int width, unsigned int height);

		void SetZoomLevel(float zoomlevel);

		glm::vec4 WindowToCameraCoordinates(const glm::vec2& position);

		void OnUpdate(Timestep timestep);

	private:
		void UpdateZoomLevel(float zoomlevel);
		void CalculateProjection();

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

		bool m_AllowRotation;

		const float m_CameraAngleStep = 20.0f;

		OrthographicCameraControllerMoveMode m_CameraMoveMode = OrthographicCameraControllerMoveMode::None;
		glm::vec3 m_MousePressedPositionWorld = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_MousePressedPositionWorldRotation = { 0.0f, 0.0f };

		const float m_ReponseTime = 1.0f / 15.0f;
		float m_ZoomLevelTarget = 1.0f;
		glm::vec3 m_PositionTarget = { 0.0f, 0.0f, 0.0f };
		float m_RotationTarget = 0.0f;
	};

}
