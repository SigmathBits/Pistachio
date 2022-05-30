#pragma once

#include "Pistachio/Renderer/Camera.h"


namespace Pistachio {

	class SceneCamera : public Camera {
	public:
		SceneCamera();
		SceneCamera(float size, float nearClip = -1.0f, float farClip = 1.0f);
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);

		void SetViewportSize(unsigned int width, unsigned int height);

		inline void SetOrthographicSize(float size) { m_OrthographicSize = size; CalculateProjection(); }
		inline float OrthographicSize() const { return m_OrthographicSize; }

	private:
		void CalculateProjection();

	private:
		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

		float m_AspectRatio = 1.0f;
	};

}
