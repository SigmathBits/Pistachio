#include "pstpch.h"

#include "Common.h"

#include <cmath>


namespace Pistachio {

	float fmodulo(float value, float mod) {
		return value - std::floor(value / mod) * mod;
	}

	float wrap_rotation(float rotation) {
		return fmodulo(rotation + fpi, 2 * fpi) - fpi;
	}

}
