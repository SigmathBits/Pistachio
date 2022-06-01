#include "pstpch.h"

#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>


namespace Pistachio {

	namespace Math {

		float fmodulo(float value, float mod) {
			return value - std::floor(value / mod) * mod;
		}

		float wrap_rotation(float rotation) {
			return fmodulo(rotation + pi, 2 * pi) - pi;
		}

		bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale) {
			/// From glm::decompose in matrix_decompose.inl

			using namespace glm;

			/// Values of these template parameters for glm::mat4 and glm::vec3
			using T = float;
			constexpr glm::qualifier Q = glm::qualifier::packed_highp;

			mat4 localMatrix(transform);

			/// Normalize the matrix.
			if (epsilonEqual(localMatrix[3][3], static_cast<T>(0), epsilon<T>())) {
				return false;
			}

			/// "First, isolate perspective." - Here we just remove it.
			if (
				epsilonNotEqual(localMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(localMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(localMatrix[2][3], static_cast<T>(0), epsilon<T>())
			) {
				// Clear the perspective partition
				localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = static_cast<T>(0);
				localMatrix[3][3] = static_cast<T>(1);
			}

			// Next take care of translation (easy).
			outTranslation = vec<3, T, Q>(localMatrix[3]);
			localMatrix[3] = vec<4, T, Q>(0, 0, 0, localMatrix[3].w);

			/// Now get scale
			vec<3, T, Q> row[3];
			for (length_t i = 0; i < 3; ++i) {
				for (length_t j = 0; j < 3; ++j) {
					row[i][j] = localMatrix[i][j];
				}
			}

			// Compute X scale factor and normalize first row.
			outScale.x = length(row[0]);// v3Length(row[0]);
			row[0] = detail::scale(row[0], static_cast<T>(1));
			// Now, compute Y scale and normalize 2nd row.
			outScale.y = length(row[1]);
			row[1] = detail::scale(row[1], static_cast<T>(1));
			// Next, get Z scale and normalize 3rd row.
			outScale.z = length(row[2]);
			row[2] = detail::scale(row[2], static_cast<T>(1));

			/// Don't think we need this?
#if 0
			// At this point, the matrix (in rows[]) is orthonormal.
			// Check for a coordinate system flip.  If the determinant
			// is -1, then negate the matrix and the scaling factors.
			vec<3, T, Q> pdum3 = cross(row[1], row[2]); // v3Cross(row[1], row[2], pdum3);
			if (dot(row[0], pdum3) < 0) {
				for (length_t i = 0; i < 3; i++) {
					outScale[i] *= static_cast<T>(-1);
					row[i] *= static_cast<T>(-1);
				}
			}
#endif

			/// Finally, determine Euler rotation ourselves with the orthonormal matrix
			outRotation.y = asin(-row[0][2]);
			if (cos(outRotation.y) != 0) {
				outRotation.x = atan2(row[1][2], row[2][2]);
				outRotation.z = atan2(row[0][1], row[0][0]);
			} else {
				outRotation.x = atan2(-row[2][0], row[1][1]);
				outRotation.z = 0;
			}

			return true;
		}

	}

}