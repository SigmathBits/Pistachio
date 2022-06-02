#pragma once

#include "Pistachio/Renderer/Camera.h"

#include "Pistachio/Events/EventListener.h"


namespace Pistachio {

	enum CameraMoveMode : int {
		CAMERA_NONE = -1,
		CAMERA_PAN = 0,
		CAMERA_ROTATE = 1,
	};

	class EditorCamera : public Camera, public EventListener {
	public:
		EditorCamera() = default;
		EditorCamera(unsigned int width, unsigned int height, float verticalFOV, float nearClip, float farClip);

		void OnUpdate(Timestep timestep);

		inline glm::mat4 ViewMatrix() const { return m_ViewMatrix; }
		inline glm::mat4 ProjectionViewMatrix() const { return m_ProjectionViewMatrix; }

		void SetViewportSize(unsigned int width, unsigned int height);

		glm::mat4 Orientation() const;
		glm::vec3 LocalXDirection() const;
		glm::vec3 LocalYDirection() const;
		glm::vec3 LocalZDirection() const;

	private:
		void CalculateProjection();
		void CalculateProjectionView();
		void CalculatePosition();

		glm::vec4 ViewportDeltaToCameraDelta(const glm::vec2& position);

		bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& event);
		bool OnMouseMoved(MouseMovedEvent& event);
		bool OnMouseScrolled(MouseScrolledEvent& event);

		bool OnKeyPressed(KeyPressedEvent& event);

	private:
		unsigned int m_ViewportWidth, m_ViewportHeight;
		float m_AspectRatio;

		float m_VerticalFOV;
		float m_NearClip, m_FarClip;

		glm::mat4 m_CameraViewMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionViewMatrix;
		float m_ProjectionWidth;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		float m_Yaw = 0.0f, m_Pitch = 0.0f, m_Roll = 0.0f;  // Rotation
		float m_Distance = 10.0f;

		CameraMoveMode m_CameraMoveMode = CAMERA_NONE;
		glm::vec4 m_InitialMousePosition = { 0.0f, 0.0f, 0.0f, 1.0f };

		const float m_CameraAngleStep = 15.0f;
		const float m_ReponseTime = 1.0f / 15.0f;

		glm::vec3 m_FocalPointTarget = { 0.0f, 0.0f, 0.0f };
		float m_PitchTarget = 0.0f, m_YawTarget = 0.0f, m_RollTarget = 0.0f;
		float m_DistanceTarget = m_Distance;
	};

}