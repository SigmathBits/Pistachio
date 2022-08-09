#pragma once

#include <filesystem>


namespace Pistachio {

	namespace FileSystem {

		char* ReadBytes(const std::filesystem::path& filepath, size_t* outSize);

		std::string ReadFile(const std::filesystem::path& filepath);

	}

}