#include "pstpch.h"

#include "Buffer.h"

#include "Pistachio/Core/Core.h"

#include "Pistachio/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"


namespace Pistachio {

	unsigned int VertexBufferElement::ComponentCount() const {
		switch (Type) {
			case ShaderDataType::Float:
				return 1;
			case ShaderDataType::Float2:
				return 2;
			case ShaderDataType::Float3:
				return 3;
			case ShaderDataType::Float4:
				return 4;
			case ShaderDataType::Mat3:
				return 3 * 3;
			case ShaderDataType::Mat4:
				return 4 * 4;
			case ShaderDataType::Int:
				return 1;
			case ShaderDataType::Int2:
				return 2;
			case ShaderDataType::Int3:
				return 3;
			case ShaderDataType::Int4:
				return 4;
			case ShaderDataType::Bool:
				return 1;
			default:
				PST_CORE_ASSERT(false, "Unrecognised ShaderDataType");
				return 0;
		}
	}

	VertexBufferLayout::VertexBufferLayout(const std::initializer_list<VertexBufferElement> elements)
		: m_Elements(elements) {
		unsigned int offset = 0;
		m_Stride = 0;
		for (auto& element : m_Elements) {
			element.Offset = offset;
			offset += (unsigned int)element.Size;
			m_Stride += element.Size;
		}
	}

	Ref<VertexBuffer> VertexBuffer::Create(size_t size) {
		switch (Renderer::RenderAPI()) {
			case RendererAPI::RenderAPI::None:
				PST_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
				return nullptr;
			case RendererAPI::RenderAPI::OpenGL:
				return CreateRef<OpenGLVertexBuffer>(size);
			default:
				PST_CORE_ASSERT(false, "Unrecognised RendererAPI");
				return nullptr;
		}
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, size_t size) {
		switch (Renderer::RenderAPI()) {
			case RendererAPI::RenderAPI::None:
				PST_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
				return nullptr;
			case RendererAPI::RenderAPI::OpenGL:
				return CreateRef<OpenGLVertexBuffer>(vertices, size);
			default:
				PST_CORE_ASSERT(false, "Unrecognised RendererAPI");
				return nullptr;
		}
	}

	Ref<IndexBuffer> IndexBuffer::Create(unsigned int* indices, unsigned int count) {
		switch (Renderer::RenderAPI()) {
			case RendererAPI::RenderAPI::None:
				PST_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
				return nullptr;
			case RendererAPI::RenderAPI::OpenGL:
				return CreateRef<OpenGLIndexBuffer>(indices, count);
			default:
				PST_CORE_ASSERT(false, "Unrecognised RendererAPI");
				return nullptr;
		}
	}

}
