#include "pstpch.h"

#include "Pistachio/Core/Input.h"

#include <GLFW/glfw3.h>

#include "Pistachio/Core/Application.h"


namespace Pistachio {

	bool Input::IsKeyPressed(PistachioKey keyCode){
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Instance().CurrentWindow().NativeWindow());

		int state = glfwGetKey(window, keyCode);
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(PistachioMouseButton button) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Instance().CurrentWindow().NativeWindow());

		int state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::MousePosition()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Instance().CurrentWindow().NativeWindow());

		double x, y;
		glfwGetCursorPos(window, &x, &y);

		return { (float)x, (float)y };
	}

	float Input::MouseX() {
		auto [x, y] = MousePosition();
		return x;
	}

	float Input::MouseY() {
		auto [x, y] = MousePosition();
		return y;
	}

}
