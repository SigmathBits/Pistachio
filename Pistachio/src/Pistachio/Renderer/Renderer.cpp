#include "pstpch.h"

#include "Renderer.h"

#include "Pistachio/Renderer/Renderer2D.h"


namespace Pistachio {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::Init() {
		PST_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown() {
		RenderCommand::Shutdown();
		Renderer2D::Shutdown();
	}

	void Renderer::ResizeWindow(unsigned int width, unsigned int height) {
		RenderCommand::SetViewPort(0, 0, width, height);
	}

	void Renderer::BeginScene(const OrthographicCamera& camera) {
		m_SceneData->ProjectionViewMatrix = camera.ProjectionViewMatrix();
	}

	void Renderer::EndScene() {

	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform) {
		shader->Bind();
		shader->SetMat4("u_ProjectionViewMatrix", m_SceneData->ProjectionViewMatrix);
		shader->SetMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}
