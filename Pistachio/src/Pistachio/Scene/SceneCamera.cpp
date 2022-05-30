#include "pstpch.h"

#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Pistachio {

	SceneCamera::SceneCamera() {
	}

	SceneCamera::SceneCamera(float size, float nearClip /*= -1.0f*/, float farClip /*= 1.0f*/)
		: m_OrthographicSize(size), m_OrthographicNear(nearClip), m_OrthographicFar(farClip) {
		CalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip) {
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;

		CalculateProjection();
	}

	void SceneCamera::SetViewportSize(unsigned int width, unsigned int height) {
		m_AspectRatio = (float)width / (float)height;

		CalculateProjection();
	}

	void SceneCamera::CalculateProjection() {
		float orthoWidth = m_AspectRatio * m_OrthographicSize;
		float orthoHeight = m_OrthographicSize;

		m_Projection = glm::ortho(
			-orthoWidth * 0.5f, orthoWidth * 0.5f, 
			-orthoHeight * 0.5f, orthoHeight * 0.5f, 
			m_OrthographicNear, m_OrthographicFar
		);
	}

}
