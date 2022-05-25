#include <Pistachio.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"


class ExampleLayer : public Pistachio::Layer {
public:
	ExampleLayer()
		: Layer("Example"), m_Camera({ -1.6f, 1.6f, -0.9f, 0.9f }), m_CameraPosition(0.0f, 0.0f, 0.0f),
		m_Position(0.0f, 0.0f, 0.0f) {}

	void OnAttach() {
		/// Rendering objects

		// Vertex Array
		m_VertexArray = Pistachio::VertexArray::Create();

		// Vertex Buffer
		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		};
		Pistachio::Ref<Pistachio::VertexBuffer> vertexBuffer;
		vertexBuffer = Pistachio::VertexBuffer::Create(vertices, sizeof(vertices));

		// Layout
		vertexBuffer->SetLayout({
			{ Pistachio::ShaderDataType::Float3, "a_Position" },
			{ Pistachio::ShaderDataType::Float2, "a_TextureCoords" },
		});
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		// Index Buffer
		unsigned int indices[6] = {
			0, 1, 2,
			2, 3, 0,
		};
		Pistachio::Ref<Pistachio::IndexBuffer> indexBuffer;
		indexBuffer = Pistachio::IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		// Shaders
		m_FlatColourShader = Pistachio::Shader::Create("assets/shaders/FlatColour.glsl");
		m_TextureShader = Pistachio::Shader::Create("assets/shaders/Texture.glsl");

		// Textures
		m_Texture = Pistachio::Texture2D::Create("assets/textures/Pistachio.png");

		m_TextureShader->Bind();
		std::dynamic_pointer_cast<Pistachio::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Pistachio::Timestep timestep) override {
		const float moveSpeed = 0.5f * timestep;

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

			m_Position = { 0.0f, 0.0f, 0.0f };
		}
		
		if (Pistachio::Input::IsKeyPressed(PST_KEY_LEFT)) {
			m_Position.x -= moveSpeed;
		}
		if (Pistachio::Input::IsKeyPressed(PST_KEY_RIGHT)) {
			m_Position.x += moveSpeed;
		}
		if (Pistachio::Input::IsKeyPressed(PST_KEY_DOWN)) {
			m_Position.y -= moveSpeed;
		}
		if (Pistachio::Input::IsKeyPressed(PST_KEY_UP)) {
			m_Position.y += moveSpeed;
		}


		Pistachio::RenderCommand::SetClearColour({ .03f, 0.1f, 0.15f, 1 });
		Pistachio::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		{
			Pistachio::Renderer::BeginScene(m_Camera);

			// Grid
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

			m_FlatColourShader->Bind();
			std::dynamic_pointer_cast<Pistachio::OpenGLShader>(m_FlatColourShader)->UploadUniformFloat4("u_Colour", m_Colour);

			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 20; x++) {
					glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
					Pistachio::Renderer::Submit(m_FlatColourShader, m_VertexArray, transform);
				}
			}

			// Square
			m_Texture->Bind(0);
			glm::mat4 transform2 = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)), m_Position);
			Pistachio::Renderer::Submit(m_TextureShader, m_VertexArray, transform2);

			Pistachio::Renderer::EndScene();
		}
	}

	void OnImGuiRender() override {
		ImGui::Begin("Set Colour!");
		ImGui::ColorEdit4("Colour", glm::value_ptr(m_Colour));
		ImGui::End();
	}

private:
	Pistachio::OrthographicCamera m_Camera;

	Pistachio::Ref<Pistachio::VertexArray> m_VertexArray;

	Pistachio::Ref<Pistachio::Texture2D> m_Texture;
	Pistachio::Ref<Pistachio::Shader> m_FlatColourShader, m_TextureShader;

	glm::vec3 m_CameraPosition;
	float m_CameraRotation = 0.0f;

	glm::vec3 m_Position;
	glm::vec4 m_Colour { 0.486f, 0.686f, 0.255f, 1.0f };
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
