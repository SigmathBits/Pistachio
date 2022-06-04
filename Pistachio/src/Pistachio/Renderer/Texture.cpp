#include "pstpch.h"

#include "Texture.h"

#include "Pistachio/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"


namespace Pistachio {

	Ref<Texture2D> Texture2D::Create(unsigned int width, unsigned int height) {
		switch (Renderer::RenderAPI()) {
			case RendererAPI::RenderAPI::None:
				PST_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
				return nullptr;
			case RendererAPI::RenderAPI::OpenGL:
				return CreateRef<OpenGLTexture2D>(width, height);
			default:
				PST_CORE_ASSERT(false, "Unrecognised RendererAPI");
				return nullptr;
		}
	}

	Ref<Texture2D> Texture2D::Create(const std::string& filepath, unsigned int levels /*= 1*/) {
		switch (Renderer::RenderAPI()) {
			case RendererAPI::RenderAPI::None:
				PST_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
				return nullptr;
			case RendererAPI::RenderAPI::OpenGL:
				return CreateRef<OpenGLTexture2D>(filepath, levels);
			default:
				PST_CORE_ASSERT(false, "Unrecognised RendererAPI");
				return nullptr;
		}
	}

}
