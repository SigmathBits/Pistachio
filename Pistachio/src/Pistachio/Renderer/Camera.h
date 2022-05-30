#pragma once

#include <glm/glm.hpp>


namespace Pistachio {

	class Camera {
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: m_Projection(projection) {}

		virtual ~Camera() = default;

		const glm::mat4 Projection() const { return m_Projection; }
		
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};

}
