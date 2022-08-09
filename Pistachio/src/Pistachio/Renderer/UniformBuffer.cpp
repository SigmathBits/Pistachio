#include "pstpch.h"

#include "UniformBuffer.h"

#include "Pistachio/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"


namespace Pistachio {

	Ref<UniformBuffer> UniformBuffer::Create(size_t size, unsigned int binding) {
		switch (Renderer::RenderAPI()) {
			case RendererAPI::RenderAPI::None:
				PST_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
				return nullptr;
			case RendererAPI::RenderAPI::OpenGL:
				return CreateRef<OpenGLUniformBuffer>(size, binding);
			default:
				PST_CORE_ASSERT(false, "Unrecognised RendererAPI");
				return nullptr;
		}
	}

}
