#pragma once

#include <string>


namespace Pistachio {

	class FileDialog {
	public:
		// These must return an empty string if cencelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};

}