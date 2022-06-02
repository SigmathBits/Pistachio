#include "pstpch.h"

#include "Transform2D.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Pistachio {

	Transform2D::Transform2D(const glm::vec3& position, const glm::vec2& size /*= { 1.0f, 1.0f }*/)
		: Position(position), Size(size), m_Transform(1.0f) {
		m_Transform = glm::translate(glm::mat4(1.0f), Position);
		m_Transform = glm::scale(m_Transform, glm::vec3(Size, 1.0f));
	}

	Transform2D::Transform2D(const glm::vec2& position, const glm::vec2& size /*= { 1.0f, 1.0f }*/)
		: Position(position, 0.0f), Size(size), m_Transform(1.0f) {
		m_Transform = glm::translate(glm::mat4(1.0f), Position);
		m_Transform = glm::scale(m_Transform, glm::vec3(Size, 1.0f));
	}

	Transform2D::Transform2D(const glm::vec3& position, float rotation, const glm::vec2& size /*= { 1.0f, 1.0f }*/)
		: Position(position), Rotation(rotation), Size(size), m_Transform(1.0f) {
		m_Transform = glm::translate(glm::mat4(1.0f), Position);
		m_Transform = glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f });
		m_Transform = glm::scale(m_Transform, glm::vec3(Size, 1.0f));
	}

	Transform2D::Transform2D(const glm::vec2& position, float rotation, const glm::vec2& size /*= { 1.0f, 1.0f }*/)
		: Position(position, 0.0f), Rotation(rotation), Size(size), m_Transform(1.0f) {
		m_Transform = glm::translate(glm::mat4(1.0f), Position);
		m_Transform = glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f });
		m_Transform = glm::scale(m_Transform, glm::vec3(Size, 1.0f));
	}

}
