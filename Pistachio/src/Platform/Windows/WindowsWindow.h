#pragma once

#include "Pistachio/Core/Window.h"

#include "Pistachio/Renderer/GraphicsContext.h"


struct GLFWwindow;


namespace Pistachio {

	class WindowsWindow : public Window {
	public:
		WindowsWindow(const WindowProperties& properties);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;

		inline virtual unsigned int Width() const override { return m_Data.Width; };
		inline virtual unsigned int Height() const override { return m_Data.Height; };

		inline virtual void* NativeWindow() const override { return m_Window; };

		virtual void SetTitle(const std::string& title);

		inline virtual void SetEventCallback(const EventCallbackFunction& callback) override { m_Data.EventCallback = callback; };
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

	private:
		virtual void Init(const WindowProperties& properties);
		virtual void Shutdown();

	private:
		GLFWwindow* m_Window;

		GraphicsContext* m_Context;

		struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFunction EventCallback;
		};

		WindowData m_Data;
	};

}
