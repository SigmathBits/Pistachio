#include "pstpch.h"

#include "OrthographicCameraController.h"

#include <set>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Input.h"


namespace Pistachio {

	OrthographicCameraController::OrthographicCameraController(unsigned int width, unsigned int height, bool rotation /*= false*/)
		: EventListener({ EventType::WindowResize, EventType::KeyPressed }, EVENT_CATEGORY_MOUSE),
		m_Width(width), m_Height(height), m_AspectRatio((float)width / (float)height),
		m_Camera({ m_ZoomLevel * -m_AspectRatio, m_ZoomLevel * m_AspectRatio, -m_ZoomLevel, m_ZoomLevel }), m_Rotation(rotation) {

	}

	void OrthographicCameraController::OnUpdate(Timestep timestep) {

	}

	bool OrthographicCameraController::OnWindowResize(WindowResizeEvent& event) {
		m_Width = event.Width();
		m_Height = event.Height();
		m_AspectRatio = (float)m_Width / (float)m_Height;

		m_Camera.SetProjection({ m_ZoomLevel * -m_AspectRatio, m_ZoomLevel * m_AspectRatio, -m_ZoomLevel, m_ZoomLevel });

		return false;
	}

	bool OrthographicCameraController::OnMouseButtonPressed(MouseButtonPressedEvent& event) {
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
		if (!m_MouseLeftButtonHeld) return false;

		glm::vec2 mousePosition = { event.X(), -event.Y() };

		if (m_CameraModePan) {
			glm::vec3 dPosition(2 * m_ZoomLevel * (m_MousePressedPosition - mousePosition) / (float)m_Height, 0.0f);
			dPosition = glm::rotate(dPosition, glm::radians(m_Camera.Rotation()), { 0.0f, 0.0f, 1.0f });

			m_Camera.SetPosition(m_MousePressedCameraPosition + dPosition);
		} else {
			glm::vec2 centre = { (float)m_Width / 2, -(float)m_Height / 2 };
			float angle = glm::orientedAngle(glm::normalize(mousePosition - centre), glm::normalize(m_MousePressedPosition - centre));

			m_Camera.SetRotation(m_MousePressedRotation + glm::degrees(angle));
		}

		return false;
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& event) {
		if (Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_LEFT)) return false;

		if (m_Rotation && Input::IsKeyPressed(PST_KEY_LEFT_CONTROL)) {
			m_Camera.SetRotation(m_CameraRotationSpeed * std::round((m_Camera.Rotation() + m_CameraRotationSpeed * event.YOffset()) / m_CameraRotationSpeed));
		} else {
			m_ZoomLevel -= event.YOffset() * 0.25f;
			m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);

			m_Camera.SetProjection({ m_ZoomLevel * -m_AspectRatio, m_ZoomLevel * m_AspectRatio, -m_ZoomLevel, m_ZoomLevel });
		}

		return false;
	}

	bool OrthographicCameraController::OnKeyPressed(KeyPressedEvent& event) {
		if (Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_LEFT)) return false;

		if (event.KeyCode() == PST_KEY_R && event.RepeatCount() == 0) {
			m_Camera.SetPosition({ 0.0f, 0.0f, 0.0f });
			m_Camera.SetRotation(0.0f);
		}

		return false;
	}

}
