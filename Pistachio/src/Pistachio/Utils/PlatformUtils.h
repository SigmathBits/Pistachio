#pragma once

#include <string>


namespace Pistachio {

	class Platform {
	public:
		static float Time();
	};


	class FileDialog {
	public:
		// These must return an empty string if cancelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
	
}