#include "pstpch.h"

#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Pistachio {

	SceneCamera::SceneCamera() {

	}

	SceneCamera::SceneCamera(float size, float nearClip /*= -1.0f*/, float farClip /*= 1.0f*/)
		: m_OrthographicSize(size), m_OrthographicNearClip(nearClip), m_OrthographicFarClip(farClip) {
		CalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip) {
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveVerticalFOV = verticalFOV;
		m_PerspectiveNearClip = nearClip;
		m_PerspectiveFarClip = farClip;

		CalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip) {
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNearClip = nearClip;
		m_OrthographicFarClip = farClip;

		CalculateProjection();
	}

	void SceneCamera::SetViewportSize(unsigned int width, unsigned int height) {
		m_AspectRatio = (float)width / (float)height;

		CalculateProjection();
	}

	void SceneCamera::CalculateProjection() {
		switch (m_ProjectionType) {
			case ProjectionType::Perspective: {
				m_Projection = glm::perspective(m_PerspectiveVerticalFOV, m_AspectRatio, m_PerspectiveNearClip, m_PerspectiveFarClip);
				break;
			}

			case ProjectionType::Orthographic: {
				float orthoWidth = m_AspectRatio * m_OrthographicSize;
				float orthoHeight = m_OrthographicSize;

				m_Projection = glm::ortho(
					-orthoWidth * 0.5f, orthoWidth * 0.5f,
					-orthoHeight * 0.5f, orthoHeight * 0.5f,
					m_OrthographicNearClip, m_OrthographicFarClip
				);

				break;
			}
		}
	}

}
