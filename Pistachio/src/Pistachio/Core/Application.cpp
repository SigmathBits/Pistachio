#include "pstpch.h"

#include "Application.h"

#include "Pistachio/Core/Log.h"
#include "Pistachio/Core/Input.h"
#include "Pistachio/Core/Timestep.h"
#include "Pistachio/Core/Platform.h"

#include "Pistachio/Renderer/Renderer.h"


namespace Pistachio {

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name /*= "Pistachio App"*/, ApplicationArguments args /*= ApplicationArguments()*/)
		: EventListener(EVENT_CATEGORY_APPLICATION), m_ApplicationArguments(args) {
		PST_PROFILE_FUNCTION();

		PST_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Scoped<Window>(Window::Create(WindowProperties{ name }));
		m_Window->SetEventCallback(PST_BIND_EVENT_FUNCTION(SendEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application() {
		PST_PROFILE_FUNCTION();

		for (auto& layer : m_LayerStack) {
			layer->OnDetach();
		}

		Renderer::Shutdown();
	}

	void Application::Run() {
		PST_PROFILE_FUNCTION();

		while (m_Running) {
			PST_PROFILE_SCOPE("RunLoop Iteration");

			float time = Platform::Time();
			Timestep timestemp = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimised) {
				{
					PST_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_LayerStack) {
						layer->OnUpdate(timestemp);
					}
				}

				m_ImGuiLayer->Begin();
				{
					PST_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : m_LayerStack) {
						layer->OnImGuiRender();
					}
				}
				m_ImGuiLayer->End();
			}


			m_Window->OnUpdate();
		}
	}

	void Application::PushLayer(Layer* layer) {
		PST_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay) {
		PST_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close() {
		m_Running = false;
	}

	bool Application::OnEventAfter(Event& event) {
		PST_PROFILE_FUNCTION();

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
			(*it)->SendEvent(event);
			if (event.IsHandled()) {
				break;
			}
		}
		return false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& event) {
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& event) {
		PST_PROFILE_FUNCTION();

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

}
