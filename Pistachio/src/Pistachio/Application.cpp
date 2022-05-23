#include "pstpch.h"

#include "Application.h"

#include "Log.h"

#include <glad/glad.h>


namespace Pistachio {

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		PST_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());

		m_Window->SetEventCallback(PST_BIND_EVENT_FUNCTION(Application::OnEvent));

		// Set callbacks
		m_EventDispatcher.SetEventCallback<WindowCloseEvent>(PST_BIND_EVENT_FUNCTION(Application::OnWindowClose));
	}

	Application::~Application() {

	}

	void Application::Run() {
		while (m_Running) {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : m_LayerStack) {
				layer->OnUpdate();
			}

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& event) {
		m_EventDispatcher.Dispatch(event);

		PST_CORE_TRACE(event);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); ) {
			(*--it)->OnEvent(event);
			if (event.Handled) {
				break;
			}
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_Running = false;
		return true;
	}

	void Application::PushLayer(Layer* layer) {
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay) {
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

}
