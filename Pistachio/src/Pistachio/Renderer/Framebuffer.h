#pragma once

#include "Pistachio/Core/Core.h"


namespace Pistachio {

	struct FramebufferSpecification {
		unsigned int Width, Height;
		// FramebufferFormat Format;
		unsigned int Samples = 1;

		bool IsSwapChainTarget = false;
	};


	class Framebuffer {
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Resize(unsigned int width, unsigned int height) = 0;

		virtual unsigned int ColourAttachmentRendererID() const = 0;

		virtual const FramebufferSpecification& Specification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};

}
