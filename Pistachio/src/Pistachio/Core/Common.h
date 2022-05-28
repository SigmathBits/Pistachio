#pragma once

#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>

#include <iomanip>


namespace Pistachio {

	constexpr float fpi = glm::pi<float>();

	float fmodulo(float value, float mod);

	float wrap_rotation(float rotation);

}


template<glm::length_t L, typename T, glm::qualifier Q>
inline std::ostream& operator<<(std::ostream& ostream, const glm::vec<L, T, Q> vector) {
	ostream << "glm::vec" << L << "(" << std::setprecision(4) << vector[0];
	for (size_t i = 1; i < vector.length(); i++) {
		ostream << ", " << vector[i];
	}
	return ostream << ")";
}
