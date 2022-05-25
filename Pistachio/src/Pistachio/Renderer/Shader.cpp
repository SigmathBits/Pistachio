#include "pstpch.h"

#include "Shader.h"

#include "Pistachio/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"


namespace Pistachio {

	Ref<Shader> Shader::Create(const std::string& filepath) {
		switch (Renderer::RenderAPI()) {
			case RendererAPI::RenderAPI::None:
				PST_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			case RendererAPI::RenderAPI::OpenGL:
				return std::make_shared<OpenGLShader>(filepath);
			default:
				PST_CORE_ASSERT(false, "Unrecognised RendererAPI");
				return nullptr;
		}
	}

	Ref<Shader> Shader::Create(const std::string& vertexSource, const std::string& fragmentSource) {
		switch (Renderer::RenderAPI()) {
			case RendererAPI::RenderAPI::None:
				PST_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			case RendererAPI::RenderAPI::OpenGL:
				return std::make_shared<OpenGLShader>(vertexSource, fragmentSource);
			default: 
				PST_CORE_ASSERT(false, "Unrecognised RendererAPI");
				return nullptr;
		}
	}

}
