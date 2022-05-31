#pragma once

#include "Pistachio/Renderer/Camera.h"


namespace Pistachio {

	class SceneCamera : public Camera {
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };

	public:
		SceneCamera();
		SceneCamera(float size, float nearClip = -1.0f, float farClip = 1.0f);
		virtual ~SceneCamera() = default;

		void SetPerspective(float verticalFOV, float nearClip, float farClip);
		void SetOrthographic(float size, float nearClip, float farClip);
		
		void SetViewportSize(unsigned int width, unsigned int height);

		inline ProjectionType CurrentProjectionType() const { return m_ProjectionType; }
		inline void SetProjectionType(ProjectionType projectionType) { m_ProjectionType = projectionType; CalculateProjection(); }

		inline float PerspectiveVerticalFOV() const { return m_PerspectiveVerticalFOV; }
		inline float PerspectiveNearClip() const { return m_PerspectiveNearClip; }
		inline float PerspectiveFarClip() const { return m_PerspectiveFarClip; }
		inline void SetPerspectiveVerticalFOV(float verticalFOV) { m_PerspectiveVerticalFOV = verticalFOV; CalculateProjection(); }
		inline void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNearClip = nearClip; CalculateProjection(); }
		inline void SetPerspectiveFarClip(float farClip) { m_PerspectiveFarClip = farClip; CalculateProjection(); }

		inline float OrthographicSize() const { return m_OrthographicSize; }
		inline float OrthographicNearClip() const { return m_OrthographicNearClip; }
		inline float OrthographicFarClip() const { return m_OrthographicFarClip; }
		inline void SetOrthographicSize(float size) { m_OrthographicSize = size; CalculateProjection(); }
		inline void SetOrthographicNearClip(float nearClip) { m_OrthographicNearClip = nearClip; CalculateProjection(); }
		inline void SetOrthographicFarClip(float farClip) { m_OrthographicFarClip = farClip; CalculateProjection(); }

	private:
		void CalculateProjection();

	private:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_PerspectiveVerticalFOV = glm::radians(45.0f);
		float m_PerspectiveNearClip = 0.01f, m_PerspectiveFarClip = 1000.0f;

		float m_OrthographicSize = 1.0f;
		float m_OrthographicNearClip = -1.0f, m_OrthographicFarClip = 1.0f;

		float m_AspectRatio = 1.0f;
	};

}
