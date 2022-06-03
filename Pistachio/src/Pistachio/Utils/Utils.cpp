#include "pstpch.h"

#include "Utils.h"


namespace Pistachio {

	namespace Utils {

		bool EndsWith(const std::string& string, const std::string& ending) {
			return string.compare(string.length() - ending.length(), ending.length(), ending) == 0;
		}

	}

}