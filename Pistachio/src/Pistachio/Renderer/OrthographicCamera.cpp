#include "pstpch.h"

#include "OrthographicCamera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "Pistachio/Math/Math.h"


namespace Pistachio {

	OrthographicCamera::OrthographicCamera(glm::vec3 position, float rotation, glm::vec4 bounds) 
		: m_ProjectionMatrix(glm::ortho(bounds.x, bounds.y, bounds.z, bounds.w, 1.0f, -1.0f)),
		m_Position(position), m_Rotation(Math::wrap_rotation(rotation)) {
		PST_PROFILE_FUNCTION();

		RecalculateProjectionViewMatrix();
	}
	
	OrthographicCamera::OrthographicCamera(glm::vec4 bounds) 
		: m_CameraViewMatrix(1.0f),
		m_ProjectionMatrix(glm::ortho(bounds.x, bounds.y, bounds.z, bounds.w, 1.0f, -1.0f)), 
		m_Position(0.0f, 0.0f, 0.0f), m_Rotation(0.0f) {
		PST_PROFILE_FUNCTION();

		m_ProjectionViewMatrix = m_ProjectionMatrix * glm::inverse(m_CameraViewMatrix);
	}
	
	OrthographicCamera::OrthographicCamera() 
		: m_CameraViewMatrix(1.0f),
		m_ProjectionMatrix(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f)), 
		m_Position(0.0f, 0.0f, 0.0f), m_Rotation(0.0f) {
		m_ProjectionViewMatrix = m_ProjectionMatrix * glm::inverse(m_CameraViewMatrix);
	}

	OrthographicCamera::~OrthographicCamera() {
	}

	void OrthographicCamera::SetProjection(glm::vec4 bounds) {
		PST_PROFILE_FUNCTION();

		m_ProjectionMatrix = glm::ortho(bounds.x, bounds.y, bounds.z, bounds.w, 1.0f, -1.0f);
		m_ProjectionViewMatrix = m_ProjectionMatrix * glm::inverse(m_CameraViewMatrix);
	}

	void OrthographicCamera::RecalculateProjectionViewMatrix() {
		PST_PROFILE_FUNCTION();

		m_CameraViewMatrix = glm::translate(glm::mat4(1.0f), m_Position);
		m_CameraViewMatrix = glm::rotate(m_CameraViewMatrix, m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f));

		m_ProjectionViewMatrix = m_ProjectionMatrix * glm::inverse(m_CameraViewMatrix);
	}

}
