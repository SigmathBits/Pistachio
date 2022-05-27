#include "pstpch.h"

#include "Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Pistachio/Renderer/RenderCommand.h"
#include "Pistachio/Renderer/VertexArray.h"
#include "Pistachio/Renderer/Shader.h"


namespace Pistachio {

	struct Renderer2DData {
		Ref<VertexArray> VertexArray;
		Ref<Shader> ColourTextureShader;
		Ref<Texture> WhiteTexture;
	};

	static Renderer2DData* s_Data;

	void Renderer2D::Init() {
		/// Rendering objects
		s_Data = new Renderer2DData();

		// Vertex Array
		s_Data->VertexArray = VertexArray::Create();

		// Vertex Buffer
		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		};
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));

		// Layout
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TextureCoords" },
		});
		s_Data->VertexArray->AddVertexBuffer(vertexBuffer);

		// Index Buffer
		unsigned int indices[6] = {
			0, 1, 2,
			2, 3, 0,
		};
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int));
		s_Data->VertexArray->SetIndexBuffer(indexBuffer);

		// Default White Texture
		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		unsigned int whiteTextureData = 0xFFFFFFFF;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		// Shader
		s_Data->ColourTextureShader = Shader::Create("assets/shaders/ColourTexture.glsl");

		s_Data->ColourTextureShader->Bind();
		s_Data->ColourTextureShader->SetInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown() {
		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera) {
		s_Data->ColourTextureShader->Bind();
		s_Data->ColourTextureShader->SetMat4("u_ProjectionViewMatrix", camera.ProjectionViewMatrix());
	}

	void Renderer2D::EndScene() {

	}

	void Renderer2D::DrawQuad(const glm::vec2& position, float rotation, const glm::vec2 size, const glm::vec4& colour) {
		Renderer2D::DrawQuad({ position.x, position.y, 0.0f }, rotation, size, colour);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, float rotation, const glm::vec2 size, const glm::vec4& colour) {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		transform = glm::rotate(transform, glm::radians(rotation), { 0.0f, 0.0f, 1.0f });
		transform = glm::scale(transform, glm::vec3(size, 1.0f));

		s_Data->ColourTextureShader->SetFloat4("u_Colour", colour);
		s_Data->ColourTextureShader->SetMat4("u_Transform", transform);

		s_Data->WhiteTexture->Bind(0);

		s_Data->VertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, float rotation, const glm::vec2 size, const Ref<Texture>& texture) {
		Renderer2D::DrawQuad({ position.x, position.y, 0.0f }, rotation, size, texture);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, float rotation, const glm::vec2 size, const Ref<Texture>& texture) {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		transform = glm::rotate(transform, glm::radians(rotation), { 0.0f, 0.0f, 1.0f });
		transform = glm::scale(transform, glm::vec3(size, 1.0f));

		s_Data->ColourTextureShader->SetFloat4("u_Colour", { 1.0f, 1.0f, 1.0f, 1.0f });
		s_Data->ColourTextureShader->SetMat4("u_Transform", transform);

		texture->Bind(0);

		s_Data->VertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}

}
