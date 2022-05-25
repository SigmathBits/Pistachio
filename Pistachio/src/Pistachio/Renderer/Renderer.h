#pragma once

#include "RendererAPI.h"
#include "RenderCommand.h"

#include "OrthographicCamera.h"
#include "Shader.h"


namespace Pistachio {

	class Renderer {
	public:
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::RenderAPI RenderAPI() { return RendererAPI::API(); }

	private:
		struct SceneData {
			glm::mat4 ProjectionViewMatrix;
		};

		static SceneData* m_SceneData;
	};

}
