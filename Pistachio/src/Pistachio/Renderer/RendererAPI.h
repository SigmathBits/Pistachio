#pragma once

#include <glm/vec4.hpp>

#include "Pistachio/Renderer/VertexArray.h"


namespace Pistachio {

	class RendererAPI {
	public:
		enum class RenderAPI {
			None = 0, OpenGL = 1,
		};

	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void SetViewPort(unsigned int x, unsigned int y, unsigned int width, unsigned int height) const = 0;

		virtual void SetClearColour(const glm::vec4& colour) const = 0;
		virtual void Clear() const = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) const = 0;

		inline static RenderAPI API() { return s_RenderAPI; }

	private:
		static RenderAPI s_RenderAPI;
	};

}
