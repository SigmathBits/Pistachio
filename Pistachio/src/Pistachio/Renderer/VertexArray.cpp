#include "pstpch.h"

#include "VertexArray.h"

#include "Pistachio/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"


namespace Pistachio {

	Ref<VertexArray> VertexArray::Create() {
		switch (Renderer::RenderAPI()) {
			case RendererAPI::RenderAPI::None:
				PST_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
				return nullptr;
			case RendererAPI::RenderAPI::OpenGL:
				return std::make_shared<OpenGLVertexArray>();
			default:
				PST_CORE_ASSERT(false, "Unrecognised RendererAPI");
				return nullptr;
		}
	}

}
