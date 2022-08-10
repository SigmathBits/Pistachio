#pragma once

#include <string>
#include <filesystem>


namespace Pistachio {

	class Platform {
	public:
		static float Time();
	};


	class FileDialog {
	public:
		// These must return an empty string if cencelled
		static std::filesystem::path OpenFile(const char* filter);
		static std::filesystem::path SaveFile(const char* filter);
	};

}