#pragma once

#include "Pistachio/Core/Core.h"
#include "Pistachio/Core/Window.h"
#include "Pistachio/Core/LayerStack.h"

#include "Pistachio/Events/EventListener.h"

#include "Pistachio/ImGui/ImGuiLayer.h"



namespace Pistachio {


	struct ApplicationArguments {
		int Count = 0;
		char** Arguments = nullptr;

		const char* operator[](int index) { 
			PST_CORE_ASSERT(index < Count, "Invalid argument index");
			return Arguments[index];
		}
	};

	
	class Application : public EventListener {
	public:
		Application(const std::string& name = "Pistachio App", ApplicationArguments args = ApplicationArguments());
		virtual ~Application();

		void Run();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void Close();

		inline Window& CurrentWindow() const { return *m_Window; }

		inline ImGuiLayer* BaseImGuiLayer() { return m_ImGuiLayer; }

		static inline Application& Instance() { return *s_Instance; }

		ApplicationArguments Arguments() const { return m_ApplicationArguments; }

	private:
		bool OnEventAfter(Event& event);

		bool OnWindowClose(WindowCloseEvent& event);
		bool OnWindowResize(WindowResizeEvent& event);

	private:
		ApplicationArguments m_ApplicationArguments;

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
	Application* CreateApplication(ApplicationArguments args);
}
