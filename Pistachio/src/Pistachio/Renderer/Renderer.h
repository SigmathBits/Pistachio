#pragma once

#include "Pistachio/Renderer/RendererAPI.h"
#include "Pistachio/Renderer/RenderCommand.h"

#include "Pistachio/Renderer/OrthographicCamera.h"

#include "Pistachio/Renderer/Shader.h"


namespace Pistachio {

	class Renderer {
	public:
		static void Init();

		static void ResizeWindow(unsigned int width, unsigned int height);

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		inline static RendererAPI::RenderAPI RenderAPI() { return RendererAPI::API(); }

	private:
		struct SceneData {
			glm::mat4 ProjectionViewMatrix;
		};

		static SceneData* m_SceneData;
	};

}
