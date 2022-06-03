#include "pstpch.h"

#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>


namespace Pistachio {

	OpenGLUniformBuffer::OpenGLUniformBuffer(size_t size, unsigned int binding) {
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);  // TODO: Not sure about the usage hint
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer() {
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLUniformBuffer::SetData(const void* data, size_t size, size_t offset) {
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}

}
