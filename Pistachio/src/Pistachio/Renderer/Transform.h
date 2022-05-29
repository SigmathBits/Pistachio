#pragma once

#include <glm/glm.hpp>


namespace Pistachio {

	struct Transform {
		glm::vec3 Position;
		glm::vec2 Size;

		Transform(const glm::vec3& position, const glm::vec2& size = { 1.0f, 1.0f })
			: Position(position), Size(size) {
		}

		Transform(const glm::vec2& position, const glm::vec2& size = { 1.0f, 1.0f })
			: Position(position, 0.0f), Size(size) {
		}
	};

	struct RotatedTransform : Transform {
		float Rotation;

		RotatedTransform(const glm::vec3& position, float rotation, const glm::vec2& size = { 1.0f, 1.0f })
			: Transform(position, size), Rotation(rotation) {
		}

		RotatedTransform(const glm::vec2& position, float rotation, const glm::vec2& size = { 1.0f, 1.0f })
			: Transform(position, size), Rotation(rotation) {
		}
	};

}
