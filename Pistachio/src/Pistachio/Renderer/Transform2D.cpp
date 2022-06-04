#include "pstpch.h"

#include "Transform2D.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Pistachio {

	Transform2D::Transform2D(const glm::vec3& position, const glm::vec2& size /*= { 1.0f, 1.0f }*/)
		: Position(position), Size(size) {

	}

	Transform2D::Transform2D(const glm::vec2& position, const glm::vec2& size /*= { 1.0f, 1.0f }*/)
		: Position(position, 0.0f), Size(size) {

	}

	Transform2D::Transform2D(const glm::vec3& position, float rotation, const glm::vec2& size /*= { 1.0f, 1.0f }*/)
		: Position(position), Rotation(rotation), Size(size) {

	}

	Transform2D::Transform2D(const glm::vec2& position, float rotation, const glm::vec2& size /*= { 1.0f, 1.0f }*/)
		: Position(position, 0.0f), Rotation(rotation), Size(size) {

	}

	glm::mat4 Transform2D::TransformMatrix() const {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), Position);
		if (Rotation != 0.0f) {
			transform = glm::rotate(transform, Rotation, { 0.0f, 0.0f, 1.0f });
		}
		transform = glm::scale(transform, glm::vec3(Size, 1.0f));
		return transform;
	}

}
