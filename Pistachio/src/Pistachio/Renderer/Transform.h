#pragma once

#include <glm/glm.hpp>


namespace Pistachio {

	struct Transform {
		glm::vec3 Position;
		glm::vec2 Size;

		Transform(glm::vec3 position, glm::vec2 size = {1.0f, 1.0f})
			: Position(position), Size(size) {
		}

		Transform(glm::vec2 position, glm::vec2 size = { 1.0f, 1.0f })
			: Position(position, 0.0f), Size(size) {
		}

		Transform(glm::vec3 position, float size)
			: Position(position), Size(size, size) {
		}

		Transform(glm::vec2 position, float size)
			: Position(position, 0.0f), Size(size, size) {
		}
	};

	struct RotatedTransform : Transform {
		float Rotation;

		RotatedTransform(glm::vec3 position, float rotation, glm::vec2 size = { 1.0f, 1.0f })
			: Transform(position, size), Rotation(rotation) {
		}

		RotatedTransform(glm::vec2 position, float rotation, glm::vec2 size = { 1.0f, 1.0f })
			: Transform(position, size), Rotation(rotation) {
		}

		RotatedTransform(glm::vec3 position, float rotation, float size)
			: Transform(position, size), Rotation(rotation) {
		}

		RotatedTransform(glm::vec2 position, float rotation, float size)
			: Transform(position, size), Rotation(rotation) {
		}
	};

}
