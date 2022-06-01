#pragma once

#include <iomanip>

#include <glm/glm.hpp>


template<glm::length_t L, typename T, glm::qualifier Q>
inline std::ostream& operator<<(std::ostream& ostream, const glm::vec<L, T, Q>& vector) {
	ostream << "glm::vec" << L << "(" << std::setprecision(4) << vector[0];
	for (size_t i = 1; i < vector.length(); i++) {
		ostream << ", " << vector[i];
	}
	return ostream << ")";
}
