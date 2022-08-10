#include "pstpch.h"

#include "Pistachio/Utils/PlatformUtils.h"

#ifdef PST_PLATFORM_WINDOWS
	#include <commdlg.h>

	// Allow us to access the native Win32 window handle from GLFW
	#define GLFW_EXPOSE_NATIVE_WIN32
	#include <GLFW/glfw3native.h>
#endif

#include <GLFW/glfw3.h>

#include "Pistachio/Core/Application.h"


namespace Pistachio {

	float Platform::Time() {
		return (float)glfwGetTime();
	}


#ifdef PST_PLATFORM_WINDOWS
	std::string FileDialog::OpenFile(const char* filter) {
		OPENFILENAMEA openFilename;  // A at the end means ASCII version
		CHAR filepathBuffer[260] = { 0 };

		// Initialize OPENFILENAME
		ZeroMemory(&openFilename, sizeof(OPENFILENAME));
		openFilename.lStructSize = sizeof(OPENFILENAME);
		openFilename.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Instance().CurrentWindow().NativeWindow());
		openFilename.lpstrFile = filepathBuffer;
		openFilename.nMaxFile = sizeof(filepathBuffer);
		openFilename.lpstrFilter = filter;
		openFilename.nFilterIndex = 1;
		openFilename.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&openFilename) == TRUE) {
			return openFilename.lpstrFile;
		}

		return std::string();
	}

	std::string FileDialog::SaveFile(const char* filter) {
		OPENFILENAMEA openFilename;  // A at the end means ASCII version
		CHAR filepathBuffer[260] = { 0 };

		// Initialize OPENFILENAME
		ZeroMemory(&openFilename, sizeof(OPENFILENAME));
		openFilename.lStructSize = sizeof(OPENFILENAME);
		openFilename.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Instance().CurrentWindow().NativeWindow());
		openFilename.lpstrFile = filepathBuffer;
		openFilename.nMaxFile = sizeof(filepathBuffer);
		openFilename.lpstrFilter = filter;
		openFilename.nFilterIndex = 1;
		openFilename.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetSaveFileNameA(&openFilename) == TRUE) {
			return openFilename.lpstrFile;
		}

		return std::string();
	}
#endif


#ifdef PST_PLATFORM_LINUX
	std::string FileDialog::OpenFile(const char* filter) {
		// TODO: Impliment Open File dialog for Linux
		PST_CORE_ERROR("OpenFile not yet implemented for Linux");
		return std::string();
	}

	std::string FileDialog::SaveFile(const char* filter) {
		// TODO: Impliment Save File dialog for Linux
		PST_CORE_ERROR("SaveFile not yet implemented for Linux");
		return std::string();
	}
#endif

}
