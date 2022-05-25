#pragma once

#include "Pistachio/Renderer/VertexArray.h"
#include "Pistachio/Renderer/Buffer.h"


namespace Pistachio {

	class OpenGLVertexArray : public VertexArray {
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& VertexBuffers() const override { return m_VertexBuffers; };
		virtual const std::shared_ptr<IndexBuffer>& CurrentIndexBuffer() const override { return m_IndexBuffer; };

	private:
		unsigned int m_RendererID;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};

}
