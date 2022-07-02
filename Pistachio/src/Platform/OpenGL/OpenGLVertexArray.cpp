#include "pstpch.h"

#include "OpenGLVertexArray.h"

#include <glad/glad.h>


namespace Pistachio {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type) {
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
				return GL_FLOAT;

			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
				return GL_FLOAT;

			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
				return GL_INT;

			case ShaderDataType::Bool:
				return GL_BOOL;

			default:
				PST_CORE_ASSERT(false, "Unrecognised ShaderDataType");
				return 0;
		}
	}

	OpenGLVertexArray::OpenGLVertexArray() {
		PST_PROFILE_FUNCTION();

		glGenVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		PST_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const {
		PST_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const {
		PST_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
		PST_PROFILE_FUNCTION();

		VertexBufferLayout layout = vertexBuffer->Layout();
		PST_CORE_ASSERT(layout.Elements().size(), "vertexBuffer has no layout");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		unsigned int index = 0;
		for (const auto& element : layout) {
			switch (element.Type) {
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4: {
					glEnableVertexAttribArray(index);
					glVertexAttribPointer(
						index,
						element.ComponentCount(),
						ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalised ? GL_TRUE : GL_FALSE,
						(GLsizei)layout.Stride(), (const void*)element.Offset
					);
					index++;
					break;
				}

				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4: {
					unsigned int count = element.ComponentCount();
					for (size_t i = 0; i < count; i++) {
						glEnableVertexAttribArray(index);
						glVertexAttribPointer(
							index,
							count,
							ShaderDataTypeToOpenGLBaseType(element.Type),
							element.Normalised ? GL_TRUE : GL_FALSE,
							(GLsizei)layout.Stride(), (const void*)(element.Offset + i * count * sizeof(float))
						);
						glVertexAttribDivisor(index, 1);
						index++;
					}
					break;
				}

				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool: {
					glEnableVertexAttribArray(index);
					glVertexAttribIPointer(
						index,
						element.ComponentCount(),
						ShaderDataTypeToOpenGLBaseType(element.Type),
						(GLsizei)layout.Stride(), (const void*)element.Offset
					);
					index++;
					break;
				}

				default:
					PST_CORE_ASSERT(false, "Unsupported Shader Data Type");
					break;
			}
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
		PST_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

}
