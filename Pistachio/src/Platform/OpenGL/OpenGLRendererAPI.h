#pragma once

#include "Pistachio/Renderer/RendererAPI.h"


namespace Pistachio {

	class OpenGLRendererAPI : public RendererAPI {
	public:
		virtual void SetClearColour(const glm::vec4& colour) const override;
		virtual void Clear() const override;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) const override;
	};

}
