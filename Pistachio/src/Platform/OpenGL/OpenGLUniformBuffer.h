#pragma once

#include "Pistachio/Renderer/UniformBuffer.h"


namespace Pistachio {

	class OpenGLUniformBuffer : public UniformBuffer {
	public:
		OpenGLUniformBuffer(size_t size, unsigned int binding);
		virtual ~OpenGLUniformBuffer();

		virtual void SetData(const void* data, size_t size, size_t offset = 0) override;

	private:
		unsigned int m_RendererID = 0;
	};

}