#pragma once

#include "Pistachio/Renderer/Buffer.h"


namespace Pistachio {

	class OpenGLVertexBuffer : public VertexBuffer {
	public:
		OpenGLVertexBuffer(size_t size);
		OpenGLVertexBuffer(float* vertices, size_t size);
		virtual ~OpenGLVertexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(void* data, size_t size) override;

		inline virtual void SetLayout(const VertexBufferLayout& layout) override { m_Layout = layout; };
		inline virtual const VertexBufferLayout& Layout() const override { return m_Layout; };


	private:
		unsigned int m_RendererID;
		VertexBufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer {
	public:
		OpenGLIndexBuffer(unsigned int* indices, unsigned int count);
		virtual ~OpenGLIndexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual unsigned int Count() const override { return m_Count; }

	private:
		unsigned int m_RendererID;
		unsigned int m_Count;
	};

}
