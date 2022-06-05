#pragma once

#include "Pistachio/Renderer/RendererAPI.h"


namespace Pistachio {

	class RenderCommand {
	public:
		inline static void Init() {
			s_RendererAPI->Init();
		}

		inline static void Shutdown() {
			s_RendererAPI->Shutdown();
		}

		inline static void SetViewPort(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
			s_RendererAPI->SetViewPort(x, y, width, height);
		}

		inline static void SetClearColour(const glm::vec4& colour) {
			s_RendererAPI->SetClearColour(colour);
		};


		inline static void Clear() {
			s_RendererAPI->Clear();
		};

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount = 0) {
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}

		inline static void DrawLines(const Ref<VertexArray>& vertexArray, unsigned int vertexCount, bool loop = false) {
			s_RendererAPI->DrawLines(vertexArray, vertexCount);
		}

		inline static void SetLineThickness(float thickness) {
			s_RendererAPI->SetLineThickness(thickness);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};

}
