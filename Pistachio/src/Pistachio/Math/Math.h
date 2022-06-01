#pragma once

#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>


namespace Pistachio {

	namespace Math {

		constexpr float pi = glm::pi<float>();

		float fmodulo(float value, float mod);

		float wrap_rotation(float rotation);

		bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

	}

}
