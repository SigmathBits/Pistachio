#include "pstpch.h"

#include "OrthographicCameraController.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Pistachio/Core/Common.h"

#include "Pistachio/Core/Input.h"


namespace Pistachio {

	OrthographicCameraController::OrthographicCameraController(unsigned int width, unsigned int height, bool allowRotation /*= false*/)
		: EventListener({ EventType::WindowResize, EventType::KeyPressed }, EVENT_CATEGORY_MOUSE),
		m_Width(width), m_Height(height), m_AspectRatio((float)width / (float)height),
		m_Camera({ m_ZoomLevel * -m_AspectRatio, m_ZoomLevel * m_AspectRatio, -m_ZoomLevel, m_ZoomLevel }), 
		m_AllowRotation(allowRotation) {
	}

	void OrthographicCameraController::OnUpdate(Timestep timestep) {
		constexpr float zoomThreshold = 0.01f;
		constexpr float rotationThreshold = 0.001f * pi;
		constexpr float positionThreshold = 0.01f;

		const float delta = std::min(timestep / m_ReponseTime, 1.0f);

		if (m_ZoomLevel != m_ZoomLevelTarget) {
			const float zoomDiff = (m_ZoomLevelTarget - m_ZoomLevel);
			if (std::abs(zoomDiff) < zoomThreshold) {
				SetZoom(m_ZoomLevelTarget);
			} else {
				SetZoom(m_ZoomLevel + zoomDiff * delta);
			}
		}
		
		const float rotation = m_Camera.Rotation();
		if (rotation != m_RotationTarget) {
			const float rotationDiff = wrap_rotation(m_RotationTarget - rotation);
			if (std::abs(rotationDiff) < rotationThreshold) {
				m_Camera.SetRotation(m_RotationTarget);
			} else {
				m_Camera.SetRotation(rotation + rotationDiff * delta);
			}
		}

		const glm::vec3 position = m_Camera.Position();
		if (position != m_PositionTarget) {
			const glm::vec3 positionDiff = (m_PositionTarget - position);
			if (glm::length(positionDiff) < positionThreshold) {
				m_Camera.SetPosition(m_PositionTarget);
			} else {
				m_Camera.SetPosition(position + positionDiff * delta);
			}
		}
	}

	void OrthographicCameraController::SetZoom(float zoom) {
		PST_PROFILE_FUNCTION();

		m_ZoomLevel = std::max(zoom, 0.10f);
		m_Camera.SetProjection({ m_ZoomLevel * -m_AspectRatio, m_ZoomLevel * m_AspectRatio, -m_ZoomLevel, m_ZoomLevel });
	}

	bool OrthographicCameraController::OnWindowResize(WindowResizeEvent& event) {
		PST_PROFILE_FUNCTION();

		m_Width = event.Width();
		m_Height = event.Height();
		m_AspectRatio = (float)m_Width / (float)m_Height;

		m_Camera.SetProjection({ m_ZoomLevel * -m_AspectRatio, m_ZoomLevel * m_AspectRatio, -m_ZoomLevel, m_ZoomLevel });

		return false;
	}

	bool OrthographicCameraController::OnMouseButtonPressed(MouseButtonPressedEvent& event) {
		PST_PROFILE_FUNCTION();

		if (event.MouseButton() != PST_MOUSE_BUTTON_LEFT) return false;

		auto& [x, y] = Input::MousePosition();
		glm::vec3 mousePositionCamera = WindowToCameraCoordinates({ x, y });

		m_MousePressedPositionWorld = m_Camera.CameraViewMatrix() * glm::vec4(mousePositionCamera, 1.0f);

		m_MousePressedPositionWorldRotation = glm::normalize(m_Camera.CameraViewMatrix() * glm::vec4(mousePositionCamera, 0.0f));

		m_CameraMoveMode = m_AllowRotation && Input::IsKeyPressed(PST_KEY_LEFT_CONTROL) ? CAMERA_ROTATE : CAMERA_PAN;

		return false;
	}

	bool OrthographicCameraController::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
		if (event.MouseButton() != PST_MOUSE_BUTTON_LEFT) return false;

		m_CameraMoveMode = CAMERA_NONE;

		return false;
	}

	bool OrthographicCameraController::OnMouseMoved(MouseMovedEvent& event) {
		PST_PROFILE_FUNCTION();

		if (m_CameraMoveMode == CAMERA_NONE) return false;

		glm::vec3 mousePositionCamera = WindowToCameraCoordinates({ event.X(), event.Y() });

		if (m_CameraMoveMode == CAMERA_PAN) {
			glm::vec3 mousePositionWorld = m_Camera.CameraViewMatrix() * glm::vec4(mousePositionCamera, 1.0f);
			
			m_Camera.SetPosition(m_Camera.Position() + m_MousePressedPositionWorld - mousePositionWorld);
			m_PositionTarget = m_Camera.Position();
		} else if (m_CameraMoveMode == CAMERA_ROTATE) {
			constexpr float rotationNoEasingThreshold = pi / 8;
			const float rotationDeadzone = 0.10f * m_ZoomLevel;

			glm::vec2 mousePositionWorldRotation = m_Camera.CameraViewMatrix() * glm::vec4(mousePositionCamera, 0.0f);
			if (glm::length(mousePositionWorldRotation) < rotationDeadzone) return false;

			float dRotation = glm::orientedAngle(glm::normalize(mousePositionWorldRotation), m_MousePressedPositionWorldRotation);

			if (std::abs(dRotation) < rotationNoEasingThreshold) {
				m_Camera.SetRotation(m_Camera.Rotation() + dRotation);
				m_RotationTarget = m_Camera.Rotation();
			} else {
				m_RotationTarget = m_Camera.Rotation() + dRotation;
			}
		}

		return false;
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& event) {
		PST_PROFILE_FUNCTION();

		if (Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_LEFT)) return false;

		if (m_AllowRotation && Input::IsKeyPressed(PST_KEY_LEFT_CONTROL)) {
			float newAngle = glm::degrees(m_RotationTarget) + m_CameraAngleStep * event.YOffset();
			// Round to nearest `m_CameraAngleSpeed`
			m_RotationTarget = wrap_rotation(glm::radians(m_CameraAngleStep * std::round(newAngle / m_CameraAngleStep)));
		} else {
			m_ZoomLevelTarget -= event.YOffset() / 4.0f;
		}

		return false;
	}

	bool OrthographicCameraController::OnKeyPressed(KeyPressedEvent& event) {
		PST_PROFILE_FUNCTION();

		if (Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_LEFT)) return false;

		if (event.KeyCode() == PST_KEY_R && event.RepeatCount() == 0) {
			m_ZoomLevelTarget = 1.0f;
			m_PositionTarget = { 0.0f, 0.0f, 0.0f };
			m_RotationTarget = 0.0f;
		}

		return false;
	}

	glm::vec4 OrthographicCameraController::WindowToCameraCoordinates(const glm::vec2& position) {
		return {
			 m_ZoomLevel * (2 * position.x / (float)m_Height - m_AspectRatio),
			-m_ZoomLevel * (2 * position.y / (float)m_Height - 1),
			0.0f,
			1.0f,
		};
	}

}
