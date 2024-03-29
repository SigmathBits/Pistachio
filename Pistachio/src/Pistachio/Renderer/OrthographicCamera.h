#pragma once

#include <glm/glm.hpp>

#include "Pistachio/Math/Math.h"


namespace Pistachio {

	class OrthographicCamera {
	public:
		OrthographicCamera(glm::vec3 position, float rotation, glm::vec4 bounds);
		OrthographicCamera(glm::vec4 bounds);
		OrthographicCamera();
		~OrthographicCamera();

		void SetProjection(glm::vec4 bounds);

		const glm::mat4& ProjectionMatrix() const { return m_ProjectionMatrix;  }
		const glm::mat4& CameraViewMatrix() const { return m_CameraViewMatrix;  }
		const glm::mat4& ViewMatrix() const { return glm::inverse(m_CameraViewMatrix);  }
		const glm::mat4& ProjectionViewMatrix() const { return m_ProjectionViewMatrix;  }

		inline const glm::vec3& Position() const { return m_Position; };
		inline float Rotation() const { return m_Rotation; };

		inline void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateProjectionViewMatrix(); };
		inline void SetRotation(float rotation) { m_Rotation = Math::wrap_rotation(rotation); RecalculateProjectionViewMatrix(); };

	private:
		void RecalculateProjectionViewMatrix();

	private:
		glm::mat4 m_CameraViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ProjectionViewMatrix;

		glm::vec3 m_Position;
		float m_Rotation;
	};

}
