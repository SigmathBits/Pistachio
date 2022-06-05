#include "pstpch.h"

#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

#include "Pistachio/Core/Core.h"


namespace Pistachio {

	static void OpenGLDebugMessageCallback(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const char* message,
		const void* userParams
	) {
		switch (severity) {
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				PST_CORE_INFO("OpenGL: {}", message);
				break;
			case GL_DEBUG_SEVERITY_LOW:
				PST_CORE_WARN("OpenGL: {}", message);
				break;
			case GL_DEBUG_SEVERITY_MEDIUM:
				PST_CORE_ERROR("OpenGL: {}", message);
				break;
			case GL_DEBUG_SEVERITY_HIGH:
				PST_CORE_CRITICAL("OpenGL: {}", message);
				break;
			default:
				break;
		}

		PST_CORE_ASSERT(false, "Unrecognised OpenGL severity level");
	}

	void OpenGLRendererAPI::Init() {
		PST_PROFILE_FUNCTION();

#ifdef PST_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageCallback(OpenGLDebugMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_LINE_SMOOTH);
	}

	void OpenGLRendererAPI::Shutdown() {

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

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount /*= 0*/) const {
		vertexArray->Bind();
		unsigned int count = indexCount ? indexCount : vertexArray->CurrentIndexBuffer()->Count();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, unsigned int vertexCount) const {
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRendererAPI::SetLineThickness(float thickness) const {
		glLineWidth(thickness);
	}

}

