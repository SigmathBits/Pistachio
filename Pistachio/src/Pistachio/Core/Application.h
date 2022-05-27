#pragma once

#include "Pistachio/Core/Core.h"
#include "Pistachio/Core/Window.h"
#include "Pistachio/Core/LayerStack.h"

#include "Pistachio/Events/EventListener.h"

#include "Pistachio/ImGui/ImGuiLayer.h"



namespace Pistachio {
	
	class Application : public EventListener {
	public:
		Application();
		virtual ~Application();

		void Run();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& CurrentWindow() const { return *m_Window; }

		static inline Application& Instance() { return *s_Instance; }

	private:
		bool OnEventAfter(Event& event);

		bool OnWindowClose(WindowCloseEvent& event);
		bool OnWindowResize(WindowResizeEvent& event);

	private:
		Scoped<Window> m_Window;

		bool m_Running = true;
		bool m_Minimised = false;

		float m_LastFrameTime = 0.0f;

		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}
