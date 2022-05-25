#include "pstpch.h"

#include "Application.h"

#include "Log.h"

#include "Platform.h"

#include "Input.h"

#include "Core/Timestep.h"
#include "Renderer/Renderer.h"


namespace Pistachio {

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		PST_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Scoped<Window>(Window::Create());
		m_Window->SetEventCallback(PST_BIND_EVENT_FUNCTION(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		// Set callbacks
		m_EventDispatcher.SetEventCallback<WindowCloseEvent>(PST_BIND_EVENT_FUNCTION(Application::OnWindowClose));
	}

	Application::~Application() {

	}

	void Application::Run() {
		while (m_Running) {
			float time = Platform::Time();
			Timestep timestemp = time - m_LastFrameTime;
			m_LastFrameTime = time;

			for (Layer* layer : m_LayerStack) {
				layer->OnUpdate(timestemp);
			}

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack) {
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& event) {
		m_EventDispatcher.Dispatch(event);

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
	}

	void Application::PushOverlay(Layer* overlay) {
		m_LayerStack.PushOverlay(overlay);
	}

}
