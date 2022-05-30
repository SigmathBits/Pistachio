#include "pstpch.h"

#include "Framebuffer.h"

#include "Pistachio/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"


namespace Pistachio {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec) {
		switch (Renderer::RenderAPI()) {
			case RendererAPI::RenderAPI::None:
				PST_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
				return nullptr;
			case RendererAPI::RenderAPI::OpenGL:
				return CreateRef<OpenGLFramebuffer>(spec);
			default:
				PST_CORE_ASSERT(false, "Unrecognised RendererAPI");
				return nullptr;
		}
	}

}
