#include "pstpch.h"

#include "OrthographicCamera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>


namespace Pistachio {

	OrthographicCamera::OrthographicCamera(glm::vec3 position, float rotation, glm::vec4 culling) 
		: m_ProjectionMatrix(glm::ortho(culling.x, culling.y, culling.z, culling.w, 1.0f, -1.0f)),
		m_Position(position), m_Rotation(rotation) {
		RecalculateProjectionViewMatrix();
	}
	
	OrthographicCamera::OrthographicCamera(glm::vec4 culling) 
		: m_CameraViewMatrix(1.0f),
		m_ProjectionMatrix(glm::ortho(culling.x, culling.y, culling.z, culling.w, 1.0f, -1.0f)), 
		m_Position(0.0f, 0.0f, 0.0f), m_Rotation(0.0f) {
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

	void OrthographicCamera::SetProjection(glm::vec4 culling) {
		m_ProjectionMatrix = glm::ortho(culling.x, culling.y, culling.z, culling.w, 1.0f, -1.0f);
		m_ProjectionViewMatrix = m_ProjectionMatrix * glm::inverse(m_CameraViewMatrix);
	}

	void OrthographicCamera::RecalculateProjectionViewMatrix() {
		m_CameraViewMatrix = glm::translate(glm::mat4(1.0f), m_Position);
		m_CameraViewMatrix = glm::rotate(m_CameraViewMatrix, glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));

		m_ProjectionViewMatrix = m_ProjectionMatrix * glm::inverse(m_CameraViewMatrix);
	}

}
