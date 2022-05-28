#include "pstpch.h"

#include "OrthographicCameraController.h"

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Pistachio/Core/Input.h"


namespace Pistachio {

	OrthographicCameraController::OrthographicCameraController(unsigned int width, unsigned int height, bool rotation /*= false*/)
		: EventListener({ EventType::WindowResize, EventType::KeyPressed }, EVENT_CATEGORY_MOUSE),
		m_Width(width), m_Height(height), m_AspectRatio((float)width / (float)height),
		m_Camera({ m_ZoomLevel * -m_AspectRatio, m_ZoomLevel * m_AspectRatio, -m_ZoomLevel, m_ZoomLevel }), 
		m_Rotation(rotation) {

	}

	void OrthographicCameraController::OnUpdate(Timestep timestep) {
		SetZoom(m_ZoomLevel + (m_ZoomLevelTarget - m_ZoomLevel) * timestep / m_ReponseTime);
		
		float rotation = m_Camera.Rotation();
		m_Camera.SetRotation(rotation + (m_RotationTarget - rotation) * timestep / m_ReponseTime);

		glm::vec3 position = m_Camera.Position();
		m_Camera.SetPosition(position + (m_PositionTarget - position) * (float)timestep / m_ReponseTime);
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

		m_CameraModePan = !Input::IsKeyPressed(PST_KEY_LEFT_CONTROL);

		auto& [x, y] = Input::MousePosition();
		m_MousePressedPosition = { x, -y };

		m_MousePressedCameraPosition = m_Camera.Position();
		m_MousePressedRotation = m_Camera.Rotation();

		m_MouseLeftButtonHeld = true;

		return false;
	}

	bool OrthographicCameraController::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
		if (event.MouseButton() != PST_MOUSE_BUTTON_LEFT) return false;

		m_MouseLeftButtonHeld = false;

		return false;
	}

	bool OrthographicCameraController::OnMouseMoved(MouseMovedEvent& event) {
		PST_PROFILE_FUNCTION();

		if (!m_MouseLeftButtonHeld) return false;

		glm::vec2 mousePosition = { event.X(), -event.Y() };

		if (m_CameraModePan) {
			glm::vec3 dPosition(2 * m_ZoomLevel * (m_MousePressedPosition - mousePosition) / (float)m_Height, 0.0f);
			dPosition = glm::rotate(dPosition, m_Camera.Rotation(), { 0.0f, 0.0f, 1.0f });

			m_Camera.SetPosition(m_MousePressedCameraPosition + dPosition);
			m_PositionTarget = m_Camera.Position();
		} else {
			glm::vec2 centre = { (float)m_Width / 2, -(float)m_Height / 2 };
			float rotation = glm::orientedAngle(glm::normalize(mousePosition - centre), glm::normalize(m_MousePressedPosition - centre));

			m_Camera.SetRotation(m_MousePressedRotation + rotation);
			m_RotationTarget = m_Camera.Rotation();
		}

		return false;
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& event) {
		PST_PROFILE_FUNCTION();

		if (Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_LEFT)) return false;

		if (m_Rotation && Input::IsKeyPressed(PST_KEY_LEFT_CONTROL)) {
			//float newAngle = glm::degrees(m_Camera.Rotation()) + m_CameraAngleSpeed * event.YOffset();
			// Round to nearest `m_CameraAngleSpeed`
			//m_Camera.SetRotation(glm::radians(m_CameraAngleSpeed * std::round(newAngle / m_CameraAngleSpeed)));
			float newAngle = glm::degrees(m_RotationTarget) + m_CameraAngleSpeed * event.YOffset();
			// Round to nearest `m_CameraAngleSpeed`
			m_RotationTarget = glm::radians(m_CameraAngleSpeed * std::round(newAngle / m_CameraAngleSpeed));
		} else {
			m_ZoomLevelTarget -= event.YOffset() / 4.0f;
		}

		return false;
	}

	bool OrthographicCameraController::OnKeyPressed(KeyPressedEvent& event) {
		PST_PROFILE_FUNCTION();

		if (Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_LEFT)) return false;

		if (event.KeyCode() == PST_KEY_R && event.RepeatCount() == 0) {
			//m_Camera.SetPosition({ 0.0f, 0.0f, 0.0f });
			m_PositionTarget = { 0.0f, 0.0f, 0.0f };
			//m_Camera.SetRotation(0.0f);
			m_RotationTarget = 0.0f;
			m_ZoomLevelTarget = 1.0f;
		}

		return false;
	}

}
