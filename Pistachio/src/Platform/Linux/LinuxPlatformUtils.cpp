#include "pstpch.h"

#include "Pistachio/Utils/PlatformUtils.h"


#ifdef PST_PLATFORM_LINUX

#include <GLFW/glfw3.h>

#include "Pistachio/Core/Application.h"


namespace Pistachio {

	float Platform::Time() {
		return (float)glfwGetTime();
	}


	std::filesystem::path FileDialog::OpenFile(const char* filter) {
		// TODO: Impliment Open File dialog for Linux
		PST_CORE_ERROR("OpenFile not yet implemented for Linux");
		return std::filesystem::path();
	}

	std::filesystem::path FileDialog::SaveFile(const char* filter) {
		// TODO: Impliment Save File dialog for Linux
		PST_CORE_ERROR("SaveFile not yet implemented for Linux");
		return std::filesystem::path();
	}

}

#endif
