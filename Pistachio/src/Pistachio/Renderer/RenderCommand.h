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

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray) {
			s_RendererAPI->DrawIndexed(vertexArray);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};

}
