#include "pstpch.h"

#include "Application.h"

#include "Log.h"

#include "Input.h"

#include "Pistachio/Renderer/Renderer.h"


namespace Pistachio {

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		PST_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());

		m_Window->SetEventCallback(PST_BIND_EVENT_FUNCTION(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		// Set callbacks
		m_EventDispatcher.SetEventCallback<WindowCloseEvent>(PST_BIND_EVENT_FUNCTION(Application::OnWindowClose));


		/// Rendering objects
		// Vertex Array
		//m_VertexArray = std::make_shared<VertexArray>(VertexArray::Create());
		m_VertexArray.reset(VertexArray::Create());

		// Vertex Buffer
		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.804f, 0.914f, 0.29f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.263, 0.392, 0.09, 1.0f,
			 0.5f,  0.5f, 0.0f, 0.804f, 0.914f, 0.29f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.263, 0.392, 0.09, 1.0f,
		};
		//m_VertexBuffer = std::make_shared<VertexBuffer>(VertexBuffer::Create(vertices, sizeof(vertices)));
		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		// Layout
		VertexBufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Colour" },
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		// Index Buffer
		unsigned int indices[6] = {
			0, 1, 2,
			2, 3, 0,
		};
		//m_IndexBuffer = std::make_shared<IndexBuffer>(IndexBuffer::Create(indices, sizeof(indices)));
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		// Triangle Vertex Array
		m_VertexArrayTriangle.reset(VertexArray::Create());

		// Vertex Buffer
		float verticesTriangle[] = {
			-0.4f, -0.4f, 0.0f,
			 0.4f, -0.4f, 0.0f,
			 0.0f,  0.4f, 0.0f,
		};
		std::shared_ptr<VertexBuffer> vertexBufferTriangle;
		vertexBufferTriangle.reset(VertexBuffer::Create(verticesTriangle, sizeof(verticesTriangle)));
		// Layout
		vertexBufferTriangle->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
		});
		m_VertexArrayTriangle->AddVertexBuffer(vertexBufferTriangle);

		// Index Buffer
		unsigned int indicesTriangle[3] = {
			0, 1, 2,
		};
		std::shared_ptr<IndexBuffer> indexBufferTriangle;
		indexBufferTriangle.reset(IndexBuffer::Create(indicesTriangle, sizeof(indicesTriangle) / sizeof(unsigned int)));
		m_VertexArrayTriangle->SetIndexBuffer(indexBufferTriangle);

		/// Shaders
		std::string vertexSource = R"glsl(
#version 330 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Colour;

out vec4 v_Position;
out vec4 v_Colour;

void main() {
	v_Position = a_Position;
	v_Colour = a_Colour;

	gl_Position = a_Position;
}
)glsl";

		std::string fragmentSource = R"glsl(
#version 330 core

in vec4 v_Position;
in vec4 v_Colour;

layout(location = 0) out vec4 colour;

void main () {
	//colour = vec4(0.075f, 0.22f, 0.247f, 1.0f);
	//colour = 0.5f + v_Position;
	colour = v_Colour;
}
)glsl";

		//m_Shader = std::make_shared<Shader>(Shader::Create(vertexSource, fragmentSource));
		m_Shader.reset(Shader::Create(vertexSource, fragmentSource));

		std::string vertexSourceBlue = R"glsl(
#version 330 core

layout(location = 0) in vec4 a_Position;

out vec4 v_Position;

void main() {
	v_Position = a_Position;

	gl_Position = a_Position;
}
)glsl";

		std::string fragmentSourceBlue = R"glsl(
#version 330 core

in vec4 v_Position;

layout(location = 0) out vec4 colour;

void main () {
	colour = vec4(0.075f, 0.22f, 0.247f, 1.0f);
}
)glsl";

		//m_Shader = std::make_shared<Shader>(Shader::Create(vertexSource, fragmentSource));
		m_ShaderBlue.reset(Shader::Create(vertexSourceBlue, fragmentSourceBlue));
	}

	Application::~Application() {

	}

	void Application::Run() {
		while (m_Running) {
			RenderCommand::SetClearColour({ .03f, 0.1f, 0.15f, 1 });
			RenderCommand::Clear();

			{
				Renderer::BeginScene();

				m_Shader->Bind();
				Renderer::Submit(m_VertexArray);

				m_ShaderBlue->Bind();
				Renderer::Submit(m_VertexArrayTriangle);

				Renderer::EndScene();
			}

			for (Layer* layer : m_LayerStack) {
				layer->OnUpdate();
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
		PST_CORE_TRACE(event);

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
