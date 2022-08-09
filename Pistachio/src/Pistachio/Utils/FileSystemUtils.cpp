#include "pstpch.h"
#include "FileSystemUtils.h"


namespace Pistachio {

	namespace FileSystem {

		char* ReadBytes(const std::filesystem::path& filepath, size_t* outSize) {
			std::ifstream inFile(filepath, std::ios::binary | std::ios::ate);

			if (!inFile) {
				PST_CORE_ERROR("Could not open file \"{}\"", filepath);
				return nullptr;
			}

			std::streampos end = inFile.tellg();
			inFile.seekg(0, std::ios::beg);
			size_t size = end - inFile.tellg();

			if (size == 0) {
				PST_CORE_ERROR("The file \"{}\" is empty", filepath);
				return nullptr;
			}

			char* buffer = new char[size];
			inFile.read((char*)buffer, size);
			inFile.close();

			*outSize = size;
			return buffer;
		}

		std::string ReadFile(const std::filesystem::path& filepath) {
			PST_PROFILE_FUNCTION();

			std::string result;
			std::ifstream inFile(filepath, std::ios::in | std::ios::binary);

			if (!inFile) {
				PST_CORE_ERROR("Could not open file \"{}\"", filepath);
				return "";
			}

			inFile.seekg(0, std::ios::end);
			result.resize(inFile.tellg());
			inFile.seekg(0, std::ios::beg);

			inFile.read(&result[0], result.size());

			inFile.close();

			return result;
		}

	}

}