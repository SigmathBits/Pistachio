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
		PST_PROFILE_FUNCTION();

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
		s_Data->ColourTextureShader = Shader::Create("assets/shaders/Texture.glsl");

		s_Data->ColourTextureShader->Bind();
		s_Data->ColourTextureShader->SetInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown() {
		PST_PROFILE_FUNCTION();

		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera) {
		PST_PROFILE_FUNCTION();

		s_Data->ColourTextureShader->Bind();
		s_Data->ColourTextureShader->SetMat4("u_ProjectionViewMatrix", camera.ProjectionViewMatrix());
	}

	void Renderer2D::EndScene() {
		PST_PROFILE_FUNCTION();

	}

	void Renderer2D::DrawQuad(const Transform& transform, const glm::vec4& colour) {
		PST_PROFILE_FUNCTION();

		glm::mat4 transformMatrix = glm::translate(glm::mat4(1.0f), transform.Position);
		transformMatrix = glm::scale(transformMatrix, glm::vec3(transform.Size, 1.0f));

		s_Data->ColourTextureShader->SetFloat4("u_Colour", colour);
		s_Data->ColourTextureShader->SetMat4("u_Transform", transformMatrix);
		s_Data->ColourTextureShader->SetFloat("u_TilingFactor", 1.0f);

		s_Data->WhiteTexture->Bind(0);

		s_Data->VertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}

	void Renderer2D::DrawQuad(const Transform& transform, const Sprite& sprite) {
		PST_PROFILE_FUNCTION();

		glm::mat4 transformMatrix = glm::translate(glm::mat4(1.0f), transform.Position);
		transformMatrix = glm::scale(transformMatrix, glm::vec3(transform.Size, 1.0f));

		s_Data->ColourTextureShader->SetFloat4("u_Colour", sprite.TintColour);
		s_Data->ColourTextureShader->SetMat4("u_Transform", transformMatrix);
		s_Data->ColourTextureShader->SetFloat("u_TilingScale", sprite.TilingScale);

		sprite.Texture->Bind(0);

		s_Data->VertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}

	void Renderer2D::DrawQuad(const RotatedTransform& transform, const glm::vec4& colour) {
		PST_PROFILE_FUNCTION();

		glm::mat4 transformMatrix = glm::translate(glm::mat4(1.0f), transform.Position);
		transformMatrix = glm::rotate(transformMatrix, transform.Rotation, { 0.0f, 0.0f, 1.0f });
		transformMatrix = glm::scale(transformMatrix, glm::vec3(transform.Size, 1.0f));

		s_Data->ColourTextureShader->SetFloat4("u_Colour", colour);
		s_Data->ColourTextureShader->SetMat4("u_Transform", transformMatrix);
		s_Data->ColourTextureShader->SetFloat("u_TilingFactor", 1.0f);

		s_Data->WhiteTexture->Bind(0);

		s_Data->VertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}

	void Renderer2D::DrawQuad(const RotatedTransform& transform, const Sprite& sprite) {
		PST_PROFILE_FUNCTION();

		glm::mat4 transformMatrix = glm::translate(glm::mat4(1.0f), transform.Position);
		transformMatrix = glm::rotate(transformMatrix, transform.Rotation, { 0.0f, 0.0f, 1.0f });
		transformMatrix = glm::scale(transformMatrix, glm::vec3(transform.Size, 1.0f));

		s_Data->ColourTextureShader->SetFloat4("u_Colour", sprite.TintColour);
		s_Data->ColourTextureShader->SetMat4("u_Transform", transformMatrix);
		s_Data->ColourTextureShader->SetFloat("u_TilingScale", sprite.TilingScale);

		sprite.Texture->Bind(0);

		s_Data->VertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}

}
