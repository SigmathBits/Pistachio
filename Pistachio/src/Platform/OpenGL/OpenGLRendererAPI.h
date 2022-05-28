#pragma once

#include "Pistachio/Renderer/RendererAPI.h"


namespace Pistachio {

	class OpenGLRendererAPI : public RendererAPI {
	public:
		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void SetViewPort(unsigned int x, unsigned int y, unsigned int width, unsigned int height) const override;

		virtual void SetClearColour(const glm::vec4& colour) const override;
		virtual void Clear() const override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount = 0) const override;
	};

}
