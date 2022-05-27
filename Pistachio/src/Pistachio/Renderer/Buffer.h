#pragma once

#include "Pistachio/Core/Core.h"


namespace Pistachio {

	enum class ShaderDataType {
		None = 0,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Bool,
	};

	static size_t ShaderDataTypeSize(ShaderDataType type) {
		switch (type) {
			case ShaderDataType::Float:
				return 1 * 4;
			case ShaderDataType::Float2:
				return 2 * 4;
			case ShaderDataType::Float3:
				return 3 * 4;
			case ShaderDataType::Float4:
				return 4 * 4;
			case ShaderDataType::Mat3:
				return 3 * 3 * 4;
			case ShaderDataType::Mat4:
				return 4 * 4 * 4;
			case ShaderDataType::Int:
				return 1 * 4;
			case ShaderDataType::Int2:
				return 2 * 4;
			case ShaderDataType::Int3:
				return 3 * 4;
			case ShaderDataType::Int4:
				return 4 * 4;
			case ShaderDataType::Bool:
				return 1;
			default:
				PST_CORE_ASSERT(false, "Unrecognised ShaderDataType");
				return 0;
		}
	}

	struct VertexBufferElement {
		ShaderDataType Type;
		std::string Name;
		size_t Size;
		size_t Offset;
		bool Normalised;

		VertexBufferElement(ShaderDataType type, const std::string& name, bool normalised = false)
			: Type(type), Name(name), Size(ShaderDataTypeSize(type)), Offset(0), Normalised(normalised) {}

		unsigned int ComponentCount() const;
	};

	class VertexBufferLayout {
	public:
		VertexBufferLayout() = default;

		VertexBufferLayout(const std::initializer_list<VertexBufferElement> elements);

		inline size_t Stride() const { return m_Stride; }

		inline const std::vector<VertexBufferElement>& Elements() const { return m_Elements; }
		
		std::vector<VertexBufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<VertexBufferElement>::iterator end() { return m_Elements.end(); }

		std::vector<VertexBufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<VertexBufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		std::vector<VertexBufferElement> m_Elements;
		size_t m_Stride = 0;
	};

	class VertexBuffer {
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetLayout(const VertexBufferLayout& layout) = 0;
		virtual const VertexBufferLayout& Layout() const = 0;

		static Ref<VertexBuffer> Create(float* vertices, size_t size);
	};


	class IndexBuffer {
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual unsigned int Count() const = 0;

		static Ref<IndexBuffer> Create(unsigned int* indices, unsigned int count);
	};

}
