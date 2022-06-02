#pragma once

#include <glm/glm.hpp>



namespace Pistachio {

	struct Transform2D {
		glm::vec3 Position;
		float Rotation = 0.0f;
		glm::vec2 Size;
		
		Transform2D(const glm::vec3& position, const glm::vec2& size = { 1.0f, 1.0f });
		Transform2D(const glm::vec2& position, const glm::vec2& size = { 1.0f, 1.0f });

		Transform2D(const glm::vec3& position, float rotation, const glm::vec2& size = { 1.0f, 1.0f });
		Transform2D(const glm::vec2& position, float rotation, const glm::vec2& size = { 1.0f, 1.0f });

		glm::mat4 TransformMatrix() const { return m_Transform; }

	private:
		glm::mat4 m_Transform;
	};

}
