#include "pstpch.h"

#include "WindowsInput.h"

#include <GLFW/glfw3.h>

#include "Pistachio/Core/Application.h"


namespace Pistachio {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(PistachioKey keyCode){
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Instance().CurrentWindow().NativeWindow());

		int state = glfwGetKey(window, keyCode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(PistachioMouseButton button) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Instance().CurrentWindow().NativeWindow());

		int state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> WindowsInput::MousePositionImpl()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Instance().CurrentWindow().NativeWindow());

		double x, y;
		glfwGetCursorPos(window, &x, &y);

		return { (float)x, (float)y };
	}

	float WindowsInput::MouseXImpl() {
		auto [x, y] = MousePositionImpl();
		return x;
	}

	float WindowsInput::MouseYImpl() {
		auto [x, y] = MousePositionImpl();
		return y;
	}

}
