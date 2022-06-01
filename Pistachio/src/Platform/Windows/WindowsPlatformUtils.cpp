#include "pstpch.h"

#include "Pistachio/Utils/PlatformUtils.h"

#include <commdlg.h>

#include <GLFW/glfw3.h>

// Allow us to access the native Win32 window handle from GLFW
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Pistachio/Core/Application.h"


namespace Pistachio {

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

}
