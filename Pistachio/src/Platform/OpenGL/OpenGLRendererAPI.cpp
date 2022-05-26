#include "pstpch.h"

#include "OpenGLRendererAPI.h"

#include <glad/glad.h>


namespace Pistachio {

	void OpenGLRendererAPI::Init() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRendererAPI::SetViewPort(unsigned int x, unsigned int y, unsigned int width, unsigned int height) const {
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColour(const glm::vec4& colour) const {
		glClearColor(colour.r, colour.g, colour.b, colour.a);
	}

	void OpenGLRendererAPI::Clear() const {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray) const {
		glDrawElements(GL_TRIANGLES, vertexArray->CurrentIndexBuffer()->Count(), GL_UNSIGNED_INT, nullptr);
	}

}

