#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <imgui.h>


// Log overloads for glm types
template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& ostream, const glm::vec<L, T, Q>& vector) {
	return ostream << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& ostream, const glm::mat<C, R, T, Q>& matrix) {
	return ostream << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& ostream, const glm::qua<T, Q>& quaternion) {
	return ostream << glm::to_string(quaternion);
}


// Log overloads for ImGui vector types
template<typename OStream>
inline OStream& operator<<(OStream& ostream, const ImVec2& vector) {
	return ostream << "ImVec2(" << vector.x << ", " << vector.y << ")";
}

template<typename OStream>
inline OStream& operator<<(OStream& ostream, const ImVec4& vector) {
	return ostream << "ImVec4(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";
}