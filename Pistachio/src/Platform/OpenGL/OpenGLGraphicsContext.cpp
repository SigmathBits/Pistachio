#include "pstpch.h"

#include "OpenGLGraphicsContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>


namespace Pistachio {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle) {
		PST_CORE_ASSERT(windowHandle, "Window handle is null")
	}

	void OpenGLContext::Init() {
		glfwMakeContextCurrent(m_WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		PST_CORE_ASSERT(status, "Failed to initialise Glad");

		PST_CORE_INFO("OpenGL Info");
		PST_CORE_INFO("  Vendor: {}", glGetString(GL_VENDOR));
		PST_CORE_INFO("  Renderer: {}", glGetString(GL_RENDERER));
		PST_CORE_INFO("  Version: {}", glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers() {
		glfwSwapBuffers(m_WindowHandle);
	}

}
