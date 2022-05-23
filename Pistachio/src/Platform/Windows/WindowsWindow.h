#pragma once

#include <GLFW/glfw3.h>

#include "Pistachio/Window.h"


namespace Pistachio {

	class WindowsWindow : public Window {
	public:
		WindowsWindow(const WindowProperties& properties);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;

		virtual unsigned int Width() const override { return m_Data.Width; };
		virtual unsigned int Height() const override { return m_Data.Height;  };

		virtual void SetEventCallback(const EventCallbackFunction& callback) override;
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

	private:
		virtual void Init(const WindowProperties& properties);
		virtual void Shutdown();

	private:
		GLFWwindow* m_Window;

		struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFunction EventCallback;
		};

		WindowData m_Data;
	};

}
