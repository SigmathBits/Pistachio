#include "pstpch.h"

#include "OpenGLGraphicsContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>


namespace Pistachio {

	OpenGLGraphicsContext::OpenGLGraphicsContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle) {
		PST_CORE_ASSERT(windowHandle, "Window handle is null")
	}

	void OpenGLGraphicsContext::Init() {
		glfwMakeContextCurrent(m_WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		PST_CORE_ASSERT(status, "Failed to initialise Glad");
	}

	void OpenGLGraphicsContext::SwapBuffers() {
		glfwSwapBuffers(m_WindowHandle);
	}

}
