#include "pstpch.h"

#include "WindowsWindow.h"

#include <GLFW/glfw3.h>

#include "Pistachio/Core/Core.h"

#include "Pistachio/Events/ApplicationEvent.h"
#include "Pistachio/Events/MouseEvent.h"
#include "Pistachio/Events/KeyEvent.h"

#include "Platform/OpenGL/OpenGLGraphicsContext.h"


namespace Pistachio {

	static bool s_GLFWInitialised = false;

	static void GLFWErrorCallback(int error, const char* description) {
		PST_CORE_ERROR("GLFW Error 0x{0:x}: {1}", error, description);
	}

	Window* Window::Create(const WindowProperties& properties) {
		return new WindowsWindow(properties);
	}

	WindowsWindow::WindowsWindow(const WindowProperties& properties) {
		Init(properties);
	}

	WindowsWindow::~WindowsWindow() {
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProperties& properties) {
		m_Data.Title = properties.Title;
		m_Data.Width = properties.Width;
		m_Data.Height = properties.Height;

		PST_CORE_INFO("Creating window {0} ({1}, {2})", properties.Title, properties.Width, properties.Height);

		if (!s_GLFWInitialised) {
			int success = glfwInit();
			PST_CORE_ASSERT(success, "Could not initialise GLFW!");

			glfwSetErrorCallback(GLFWErrorCallback);

			s_GLFWInitialised = true;
		}

		m_Window = glfwCreateWindow(properties.Width, properties.Height, properties.Title.c_str(), nullptr, nullptr);

		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();

		// What data is passed to callback functions set with GLFW
		glfwSetWindowUserPointer(m_Window, &m_Data);

		SetVSync(true);

		/// Set up all GLFW event callbacks
		// Application Events
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			if (focused) {
				WindowFocusEvent event;
				data.EventCallback(event);
			} else {
				WindowLostFocusEvent event;
				data.EventCallback(event);
			}
		});

		// Key Events
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS: {
					KeyPressedEvent event((PistachioKey)key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					KeyReleasedEvent event((PistachioKey)key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT: {
					KeyPressedEvent event((PistachioKey)key, 1);
					data.EventCallback(event);
					break;
				}
				default: {
					PST_CORE_ASSERT(false, "Invalid Key Action");
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int character) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(character);
			data.EventCallback(event);
		});

		// Mouse Events
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS: {
					MouseButtonPressedEvent event((PistachioMouseButton)button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					MouseButtonReleasedEvent event((PistachioMouseButton)button);
					data.EventCallback(event);
					break;
				}
				default: {
					PST_CORE_ASSERT(false, "Invalid Button Action");
					break;
				}
			}
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)x, (float)y);
			data.EventCallback(event);
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});
	}

	void WindowsWindow::Shutdown() {
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate() {
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled) {
		if (enabled) {
			glfwSwapInterval(1);
		} else {
			glfwSwapInterval(0);
		}

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const {
		return m_Data.VSync;
	}

}
