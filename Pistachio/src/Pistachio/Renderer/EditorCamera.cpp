#include "pstpch.h"

#include "EditorCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Pistachio/Core/Input.h"

#include "Pistachio/Math/Math.h"

#include "Pistachio/Core/LogOverloads.h"


namespace Pistachio {

	namespace Utils {

		// Workaround because bug with G++ 9.4.0 prevents use of std::fabsf ...
		static float fabsf(float value) {
			return std::abs(value);
		}

	}


	template<typename T>
	static bool easeTowards(T& value, const T target, float threshold, float delta,
		std::function<float(T)> absFunction = Utils::fabsf, std::function<T(T)> diffFunction = [](T a) {return a; }) {
		if (value != target) {
			const T distanceDiff = diffFunction(target - value);
			if (absFunction(distanceDiff) < threshold) {
				value = target;
			} else {
				value += distanceDiff * delta;
			}
			return true;
		}
		return false;
	}


	EditorCamera::EditorCamera(unsigned int width, unsigned int height, float verticalFOV, float nearClip, float farClip)
		: EventListener({ EventType::KeyPressed }, EVENT_CATEGORY_MOUSE),
		m_ViewportWidth(width), m_ViewportHeight(height), m_AspectRatio((float)width / (float)height),
		m_VerticalFOV(verticalFOV), m_NearClip(nearClip), m_FarClip(farClip),
		Camera(glm::perspective(verticalFOV, m_AspectRatio, nearClip, farClip)) {
		CalculateProjectionView();
	}

	void EditorCamera::OnUpdate(Timestep timestep) {
		constexpr float focalPointThreshold = 0.01f;
		constexpr float rotationThreshold = 0.001f * Math::pi;
		constexpr float distanceThreshold = 0.01f;

		const float delta = std::min(timestep / m_ReponseTime, 1.0f);

		bool changed = false;

		changed |= easeTowards<glm::vec3>(m_FocalPoint, m_FocalPointTarget, focalPointThreshold, delta, [](glm::vec3 vector) { return glm::length(vector); });

		changed |= easeTowards<float>(m_Pitch, m_PitchTarget, rotationThreshold, delta, Utils::fabsf, Math::wrap_rotation);
		changed |= easeTowards<float>(m_Yaw, m_YawTarget, rotationThreshold, delta, Utils::fabsf, Math::wrap_rotation);
		changed |= easeTowards<float>(m_Roll, m_RollTarget, rotationThreshold, delta, Utils::fabsf, Math::wrap_rotation);

		changed |= easeTowards<float>(m_Distance, m_DistanceTarget, distanceThreshold, delta);

		if (changed) {
			CalculateProjectionView();
		}
	}

	void EditorCamera::SetFocalPlane(const glm::vec3& focalPointOnPlane) {
		auto focalZ = (m_ViewMatrix * glm::vec4(focalPointOnPlane, 1.0f)).z;
		m_Distance = -focalZ;
		m_FocalPoint = m_CameraViewMatrix * glm::vec4(0.0f, 0.0f, focalZ, 1.0f);

		m_DistanceTarget = m_Distance;
		m_FocalPointTarget = m_FocalPoint;

		CalculateProjection();
	}

	void EditorCamera::SetViewportSize(unsigned int width, unsigned int height) {
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		CalculateProjection();
	}

	void EditorCamera::CalculateProjection() {
		m_AspectRatio = (float)m_ViewportWidth / (float)m_ViewportHeight;

		m_ProjectionMatrix = glm::perspective(m_VerticalFOV, m_AspectRatio, m_NearClip, m_FarClip);
		m_ProjectionViewMatrix = m_ProjectionMatrix * m_ViewMatrix;

		m_ProjectionWidth = 2.0f * (m_ProjectionMatrix * glm::vec4(0.0f, 0.0f, -m_Distance, 1.0f)).w;
	}

	void EditorCamera::CalculateProjectionView() {
		CalculatePosition();

		m_CameraViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * Orientation();
		m_ViewMatrix = glm::inverse(m_CameraViewMatrix);
		m_ProjectionViewMatrix = m_ProjectionMatrix * m_ViewMatrix;

		m_ProjectionWidth = 2.0f * (m_ProjectionMatrix * glm::vec4(0.0f, 0.0f, -m_Distance, 1.0f)).w;
	}

	void EditorCamera::CalculatePosition() {
		m_Position = m_FocalPoint + m_Distance * LocalZDirection();
	}

	glm::vec4 EditorCamera::ViewportDeltaToCameraDelta(const glm::vec2& position) {
		return {
			 m_ProjectionWidth * position.x / (float)m_ViewportWidth,
			-m_ProjectionWidth * position.y / (float)m_ViewportHeight,
			0.0f,
			0.0f,
		};
	}

	bool EditorCamera::OnMouseButtonPressed(MouseButtonPressedEvent& event) {
		PST_PROFILE_FUNCTION();

		if (event.MouseButton() != PST_MOUSE_BUTTON_MIDDLE) return false;

		auto [x, y] = Input::MousePosition();
		m_InitialMousePosition = ViewportDeltaToCameraDelta({x, y});

		m_CameraMoveMode = Input::IsKeyPressed(PST_KEY_LEFT_ALT) ? CAMERA_ROTATE : CAMERA_PAN;

		return false;
	}

	bool EditorCamera::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
		if (event.MouseButton() != PST_MOUSE_BUTTON_MIDDLE) return false;

		m_CameraMoveMode = CAMERA_NONE;

		return false;
	}

	bool EditorCamera::OnMouseMoved(MouseMovedEvent& event) {
		PST_PROFILE_FUNCTION();

		// Fix mouse down still being sent, but not mouse released, when resizing the viewport
		if (!Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_MIDDLE)) {
			m_CameraMoveMode = CAMERA_NONE;
			return false;
		}

		if (m_CameraMoveMode == CAMERA_NONE) return false;

		auto [x, y] = Input::MousePosition();
		glm::vec4 mouse = ViewportDeltaToCameraDelta({ x, y });

		if (m_CameraMoveMode == CAMERA_PAN) {
			glm::vec3 dPosition = glm::inverse(m_ProjectionViewMatrix) * (m_InitialMousePosition - mouse);
			m_InitialMousePosition = mouse;

			m_FocalPoint += dPosition;
			m_FocalPointTarget = m_FocalPoint;
			CalculateProjectionView();
		} else if (m_CameraMoveMode == CAMERA_ROTATE) {
			glm::vec2 mouseDelta = glm::rotate(glm::vec2(m_InitialMousePosition - mouse), m_Roll);
			m_InitialMousePosition = mouse;

			const float ySign = LocalYDirection().y < 0 ? -1.0f : 1.0f;

			m_Yaw += ySign * mouseDelta.x * 2 * Math::pi / m_ProjectionWidth;
			m_Pitch += mouseDelta.y * 2 * Math::pi / m_ProjectionWidth;

			m_YawTarget = m_Yaw;
			m_PitchTarget = m_Pitch;

			CalculateProjectionView();
		}

		return false;
	}

	bool EditorCamera::OnMouseScrolled(MouseScrolledEvent& event) {
		PST_PROFILE_FUNCTION();
	
		if (Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_MIDDLE)) return false;
	
		if (Input::IsKeyPressed(PST_KEY_LEFT_ALT)) {
			float newAngle = glm::degrees(m_RollTarget) + m_CameraAngleStep * event.YOffset();
			// Round to nearest `m_CameraAngleSpeed`
			m_RollTarget = Math::wrap_rotation(glm::radians(m_CameraAngleStep * std::round(newAngle / m_CameraAngleStep)));
			CalculateProjectionView();
		} else {  // Zoom
			const float zoomDelta = std::min(0.2f * m_DistanceTarget, 5.0f);
			m_DistanceTarget = std::max(m_DistanceTarget - event.YOffset() * zoomDelta, 0.10f);
		}
	
		return false;
	}

	bool EditorCamera::OnKeyPressed(KeyPressedEvent& event) {
		PST_PROFILE_FUNCTION();

		if (Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_MIDDLE)) return false;

		if (Input::IsKeyPressed(PST_KEY_LEFT_ALT) && event.KeyCode() == PST_KEY_R && !event.IsRepeated()) {
			m_FocalPointTarget = { 0.0f, 0.0f, 0.0f };

			m_YawTarget = 0.0f;
			m_PitchTarget = 0.0f;
			m_RollTarget = 0.0f;

			m_DistanceTarget = 10.0f;

			CalculateProjectionView();
		}

		return false;
	}

	glm::mat4 EditorCamera::Orientation() const {
		return glm::yawPitchRoll(m_Yaw, -m_Pitch, m_Roll);
		//return glm::toMat4(glm::quat(glm::vec3(-m_Pitch, m_Yaw, m_Roll)));
	}

	glm::vec3 EditorCamera::LocalXDirection() const {
		return Orientation() * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	glm::vec3 EditorCamera::LocalYDirection() const {
		return Orientation() * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	}

	glm::vec3 EditorCamera::LocalZDirection() const {
		return Orientation() * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}

}
