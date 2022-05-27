#include <Pistachio.h>
#include <Pistachio/Core/EntryPoint.h>

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2DLayer.h"


class ExampleLayer : public Pistachio::Layer {
private:
	Pistachio::OrthographicCameraController m_CameraController;

	Pistachio::Ref<Pistachio::VertexArray> m_VertexArray;

	Pistachio::Ref<Pistachio::Texture2D> m_Texture;

	Pistachio::ShaderLibrary m_ShaderLibrary;

	glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Colour{ 0.486f, 0.686f, 0.255f, 1.0f };

public:
	ExampleLayer()
		: Layer("Example", Pistachio::EVENT_CATEGORY_NONE), m_CameraController(1280, 720, true) {}

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
		Pistachio::Ref<Pistachio::VertexBuffer> vertexBuffer = Pistachio::VertexBuffer::Create(vertices, sizeof(vertices));

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
		Pistachio::Ref<Pistachio::IndexBuffer> indexBuffer = Pistachio::IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		// Shaders
		m_ShaderLibrary.Load("assets/shaders/FlatColour.glsl");
		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		// Textures
		m_Texture = Pistachio::Texture2D::Create("assets/textures/Pistachio.png");

		textureShader->Bind();
		textureShader->SetInt("u_Texture", 0);
	}

	void OnUpdate(Pistachio::Timestep timestep) override {
		// Update
		m_CameraController.OnUpdate(timestep);

		const float moveSpeed = 0.5f * timestep;
		
		if (Pistachio::Input::IsKeyPressed(PST_KEY_A)) {
			m_Position.x -= moveSpeed;
		}
		if (Pistachio::Input::IsKeyPressed(PST_KEY_D)) {
			m_Position.x += moveSpeed;
		}
		if (Pistachio::Input::IsKeyPressed(PST_KEY_S)) {
			m_Position.y -= moveSpeed;
		}
		if (Pistachio::Input::IsKeyPressed(PST_KEY_W)) {
			m_Position.y += moveSpeed;
		}

		// Render
		Pistachio::RenderCommand::SetClearColour({ .03f, 0.1f, 0.15f, 1 });
		Pistachio::RenderCommand::Clear();

		{
			Pistachio::Renderer::BeginScene(m_CameraController.Camera());

			// Grid
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

			auto flatColourShader = m_ShaderLibrary.Get("FlatColour");

			flatColourShader->Bind();
			flatColourShader->SetFloat4("u_Colour", m_Colour);

			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 20; x++) {
					glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
					Pistachio::Renderer::Submit(flatColourShader, m_VertexArray, transform);
				}
			}

			// Square
			auto textureShader = m_ShaderLibrary.Get("Texture");

			m_Texture->Bind(0);
			glm::mat4 transform2 = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)), m_Position);
			Pistachio::Renderer::Submit(textureShader, m_VertexArray, transform2);

			Pistachio::Renderer::EndScene();
		}
	}

	bool OnEvent(Pistachio::Event& event) {
		m_CameraController.SendEvent(event);
		return false;
	}

	void OnImGuiRender() override {
		ImGui::Begin("Set Colour!");
		ImGui::ColorEdit4("Colour", glm::value_ptr(m_Colour));
		ImGui::End();

		ImGui::ShowDemoWindow();
	}
};


class Sandbox : public Pistachio::Application {
public:
	Sandbox() {
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2DLayer());
	}

	~Sandbox() override {

	}
};


Pistachio::Application* Pistachio::CreateApplication() {
	return new Sandbox();
}
