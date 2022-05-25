#include <Pistachio.h>

#include "imgui/imgui.h"


class ExampleLayer : public Pistachio::Layer {
public:
	ExampleLayer()
		: Layer("Example"), m_Camera({ -1.6f, 1.6f, -0.9f, 0.9f }), m_CameraPosition(0.0f, 0.0f, 0.0f) { }

	void OnAttach() {
		/// Rendering objects

		// Vertex Array
		//m_VertexArray = std::make_shared<VertexArray>(VertexArray::Create());
		m_VertexArray.reset(Pistachio::VertexArray::Create());

		// Vertex Buffer
		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.804f, 0.914f, 0.29f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.263, 0.392, 0.09, 1.0f,
			 0.5f,  0.5f, 0.0f, 0.804f, 0.914f, 0.29f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.263, 0.392, 0.09, 1.0f,
		};
		//m_VertexBuffer = std::make_shared<VertexBuffer>(VertexBuffer::Create(vertices, sizeof(vertices)));
		std::shared_ptr<Pistachio::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Pistachio::VertexBuffer::Create(vertices, sizeof(vertices)));

		// Layout
		vertexBuffer->SetLayout({
			{ Pistachio::ShaderDataType::Float3, "a_Position" },
			{ Pistachio::ShaderDataType::Float4, "a_Colour" },
		});
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		// Index Buffer
		unsigned int indices[6] = {
			0, 1, 2,
			2, 3, 0,
		};
		//m_IndexBuffer = std::make_shared<IndexBuffer>(IndexBuffer::Create(indices, sizeof(indices)));
		std::shared_ptr<Pistachio::IndexBuffer> indexBuffer;
		indexBuffer.reset(Pistachio::IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		// Triangle Vertex Array
		m_VertexArrayTriangle.reset(Pistachio::VertexArray::Create());

		// Vertex Buffer
		float verticesTriangle[] = {
			-0.4f, -0.4f, 0.0f,
			 0.4f, -0.4f, 0.0f,
			 0.0f,  0.4f, 0.0f,
		};
		std::shared_ptr<Pistachio::VertexBuffer> vertexBufferTriangle;
		vertexBufferTriangle.reset(Pistachio::VertexBuffer::Create(verticesTriangle, sizeof(verticesTriangle)));
		// Layout
		vertexBufferTriangle->SetLayout({
			{ Pistachio::ShaderDataType::Float3, "a_Position" },
		});
		m_VertexArrayTriangle->AddVertexBuffer(vertexBufferTriangle);

		// Index Buffer
		unsigned int indicesTriangle[3] = {
			0, 1, 2,
		};
		std::shared_ptr<Pistachio::IndexBuffer> indexBufferTriangle;
		indexBufferTriangle.reset(Pistachio::IndexBuffer::Create(indicesTriangle, sizeof(indicesTriangle) / sizeof(unsigned int)));
		m_VertexArrayTriangle->SetIndexBuffer(indexBufferTriangle);

		/// Shaders
		std::string vertexSource = R"glsl(
#version 330 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Colour;

uniform mat4 u_ProjectionViewMatrix;

out vec4 v_Position;
out vec4 v_Colour;

void main() {
	v_Position = a_Position;
	v_Colour = a_Colour;

	gl_Position = u_ProjectionViewMatrix * a_Position;
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
		m_Shader.reset(Pistachio::Shader::Create(vertexSource, fragmentSource));

		std::string vertexSourceBlue = R"glsl(
#version 330 core

layout(location = 0) in vec4 a_Position;

uniform mat4 u_ProjectionViewMatrix;

out vec4 v_Position;

void main() {
	v_Position = a_Position;

	gl_Position = u_ProjectionViewMatrix * a_Position;
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
		m_ShaderBlue.reset(Pistachio::Shader::Create(vertexSourceBlue, fragmentSourceBlue));
	}

	void OnUpdate(Pistachio::Timestep timestep) override {
		PST_TRACE("Delta time: {}s ({}ms)", timestep.Seconds(), timestep.Milliseconds());

		const float cameraMoveSpeed = 1.5f * timestep;
		const float cameraRotationSpeed = 180.0f * timestep;

		if (Pistachio::Input::IsKeyPressed(PST_KEY_A)) {
			m_CameraPosition.x -= cameraMoveSpeed;
		}
		if (Pistachio::Input::IsKeyPressed(PST_KEY_D)) {
			m_CameraPosition.x += cameraMoveSpeed;
		}
		if (Pistachio::Input::IsKeyPressed(PST_KEY_S)) {
			m_CameraPosition.y -= cameraMoveSpeed;
		}
		if (Pistachio::Input::IsKeyPressed(PST_KEY_W)) {
			m_CameraPosition.y += cameraMoveSpeed;
		}
		if (Pistachio::Input::IsKeyPressed(PST_KEY_E)) {
			m_CameraRotation -= cameraRotationSpeed;
		}
		if (Pistachio::Input::IsKeyPressed(PST_KEY_Q)) {
			m_CameraRotation += cameraRotationSpeed;
		}
		if (Pistachio::Input::IsKeyPressed(PST_KEY_R)) {
			m_CameraPosition = { 0.0f, 0.0f, 0.0f };
			m_CameraRotation = 0.0f;
		}


		Pistachio::RenderCommand::SetClearColour({ .03f, 0.1f, 0.15f, 1 });
		Pistachio::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		{
			Pistachio::Renderer::BeginScene(m_Camera);

			Pistachio::Renderer::Submit(m_Shader, m_VertexArray);

			Pistachio::Renderer::Submit(m_ShaderBlue, m_VertexArrayTriangle);

			Pistachio::Renderer::EndScene();
		}
	}

	void OnImGuiRender() override {
		ImGui::Begin("Greetings, Equestria!");
		ImGui::Text("Ponies!");
		ImGui::End();
	}

	bool OnKeyPressed(Pistachio::KeyPressedEvent& event) override {
		//switch (event.KeyCode()) {
		//	case PST_KEY_A:
		//		m_CameraPosition.x -= 0.05f;
		//		break;
		//	case PST_KEY_D:
		//		m_CameraPosition.x += 0.05f;
		//		break;
		//	case PST_KEY_S:
		//		m_CameraPosition.y -= 0.05f;
		//		break;
		//	case PST_KEY_W:
		//		m_CameraPosition.y += 0.05f;
		//		break;
		//	case PST_KEY_E:
		//		m_CameraRotation -= 1.0f;
		//		break;
		//	case PST_KEY_Q:
		//		m_CameraRotation += 1.0f;
		//		break;
		//	case PST_KEY_R:
		//		m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		//		m_CameraRotation = 0.0f;
		//	default: 
		//		break;
		//}

		return false;
	}

private:
	Pistachio::OrthographicCamera m_Camera;

	std::shared_ptr<Pistachio::VertexArray> m_VertexArray;
	std::shared_ptr<Pistachio::VertexArray> m_VertexArrayTriangle;

	std::shared_ptr<Pistachio::Shader> m_Shader;
	std::shared_ptr<Pistachio::Shader> m_ShaderBlue;

	glm::vec3 m_CameraPosition;
	float m_CameraRotation = 0.0f;
};


class Sandbox : public Pistachio::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}

	~Sandbox() override {

	}
};


Pistachio::Application* Pistachio::CreateApplication() {
	return new Sandbox();
}
