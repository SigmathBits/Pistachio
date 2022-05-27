#pragma once

#include "Pistachio/Core/Core.h"

#include "Pistachio/Renderer/VertexArray.h"
#include "Pistachio/Renderer/Buffer.h"


namespace Pistachio {

	class OpenGLVertexArray : public VertexArray {
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& VertexBuffers() const override { return m_VertexBuffers; };
		virtual const Ref<IndexBuffer>& CurrentIndexBuffer() const override { return m_IndexBuffer; };

	private:
		unsigned int m_RendererID;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};

}
