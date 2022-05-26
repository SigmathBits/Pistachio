#include "pstpch.h"

#include "OrthographicCameraController.h"

#include <set>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Input.h"


namespace Pistachio {

	OrthographicCameraController::OrthographicCameraController(unsigned int width, unsigned int height, bool rotation /*= false*/)
		: EventListener({ EventType::WindowResize }, EVENT_CATEGORY_MOUSE),
		m_Width(width), m_Height(height), m_AspectRatio((float)width / (float)height),
		m_Camera({ m_ZoomLevel * -m_AspectRatio, m_ZoomLevel * m_AspectRatio, -m_ZoomLevel, m_ZoomLevel }), m_Rotation(rotation) {

	}

	void OrthographicCameraController::OnUpdate(Timestep timestep) {
		if (m_Rotation && !Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_LEFT)) {
			const float cameraRotationSpeed = m_CameraRotationSpeed * timestep;
			if (Input::IsKeyPressed(PST_KEY_E)) {
				m_CameraRotation -= cameraRotationSpeed;
			}
			if (Input::IsKeyPressed(PST_KEY_Q)) {
				m_CameraRotation += cameraRotationSpeed;
			}
		}

		if (Input::IsKeyPressed(PST_KEY_R)) {
			m_CameraPosition = { 0.0f, 0.0f, 0.0f };
			m_CameraRotation = 0.0f;
		}

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);
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

		m_MousePressedPosition = Input::MousePosition();
		glm::vec3 position = m_Camera.Position();
		m_MousePressedCameraPosition = { position.x, position.y };

		m_MouseLeftButtonHeld = true;

		return false;
	}

	bool OrthographicCameraController::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
		if (event.MouseButton() != PST_MOUSE_BUTTON_LEFT) return false;

		m_MouseLeftButtonHeld = false;

		return false;
	}

	bool OrthographicCameraController::OnMouseMoved(MouseMovedEvent& event) {
		// Fix for if OnMouseButtonPressed wasn't triggered on an ImGui defocus: double-check if the left mouse button is held
		if (!m_MouseLeftButtonHeld && Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_LEFT)) {
			m_MousePressedPosition = Input::MousePosition();
			glm::vec3 position = m_Camera.Position();
			m_MousePressedCameraPosition = { position.x, position.y };

			m_MouseLeftButtonHeld = true;
		}

		if (m_MouseLeftButtonHeld) {
			auto& [x, y] = m_MousePressedPosition;

			float dX = 2 * m_ZoomLevel * m_AspectRatio * (x - event.X()) / (float)m_Width;
			float dY = -2 * m_ZoomLevel * (y - event.Y()) / (float)m_Height;

			glm::vec4 dPosition =
				glm::rotate(glm::mat4(1.0f), glm::radians(m_Camera.Rotation()), { 0.0f, 0.0f, 1.0f })
				* glm::vec4(dX, dY, 0.0f, 1.0f);

			auto& [camX, camY] = m_MousePressedCameraPosition;
			m_CameraPosition = { camX + dPosition.x, camY + dPosition.y, 1.0f };
			m_Camera.SetPosition(m_CameraPosition);
		}

		return false;
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& event) {
		m_ZoomLevel -= event.YOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);

		m_Camera.SetProjection({ m_ZoomLevel * -m_AspectRatio, m_ZoomLevel * m_AspectRatio, -m_ZoomLevel, m_ZoomLevel });
		m_CameraMoveSpeed = m_ZoomLevel;

		return false;
	}

}
