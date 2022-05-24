#pragma once

#include "Pistachio/Renderer/GraphicsContext.h"


struct GLFWwindow;


namespace Pistachio {

	class OpenGLGraphicsContext : public GraphicsContext {
	public:
		OpenGLGraphicsContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};

}
