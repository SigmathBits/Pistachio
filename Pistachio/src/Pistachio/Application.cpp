#include "pstpch.h"

#include "Application.h"

#include "Log.h"

#include "Platform.h"

#include "Input.h"

#include "Core/Timestep.h"
#include "Renderer/Renderer.h"


namespace Pistachio {

	Application* Application::s_Instance = nullptr;

	Application::Application() 
		: EventListener(EVENT_CATEGORY_APPLICATION) {
		PST_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Scoped<Window>(Window::Create());
		m_Window->SetEventCallback(PST_BIND_EVENT_FUNCTION(Application::SendEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application() {

	}

	void Application::Run() {
		while (m_Running) {
			float time = Platform::Time();
			Timestep timestemp = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimised) {
				for (Layer* layer : m_LayerStack) {
					layer->OnUpdate(timestemp);
				}
			}

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack) {
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	void Application::SendEvent(Event& event) {
		EventListener::SendEvent(event);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); ) {
			EventType eventType = event.Type();
			(*--it)->SendEvent(event);
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

	bool Application::OnWindowResize(WindowResizeEvent& event) {
		unsigned int width = event.Width();
		unsigned int height = event.Height();

		if (width == 0 || height == 0) {
			m_Minimised = true;
			return false;
		}

		m_Minimised = false;

		Renderer::ResizeWindow(width, height);

		return false;
	}

	void Application::PushLayer(Layer* layer) {
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay) {
		m_LayerStack.PushOverlay(overlay);
	}

}
