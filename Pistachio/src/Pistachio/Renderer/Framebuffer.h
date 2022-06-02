#pragma once

#include "Pistachio/Core/Core.h"


namespace Pistachio {

	enum class FramebufferTextureFormat {
		None = 0,

		// Colour
		RGBA8,
		RED_INTEGER,

		// Depth/Stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8,
	};

	struct FramebufferTextureSpecification {
		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO: Filtering/Wrapping

		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat textureFormat)
			: TextureFormat(textureFormat) {}
	};

	// "Colective framebuffer attachment"
	struct FramebufferAttachmentSpecification {
		std::vector<FramebufferTextureSpecification> Attachments;

		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}
	};

	struct FramebufferSpecification {
		unsigned int Width, Height;
		FramebufferAttachmentSpecification AttachmentsSpecification;
		unsigned int Samples = 1;

		bool IsSwapChainTarget = false;
	};


	class Framebuffer {
	public:
		virtual ~Framebuffer() = default;

		virtual void ClearColourAttachment(int attachmentIndex, int value) const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Resize(unsigned int width, unsigned int height) = 0;

		virtual unsigned int ColourAttachmentRendererID(unsigned int index = 0) const = 0;
		virtual int ReadPixel(unsigned int attachmentIndex, int x, int y) const = 0;

		virtual const FramebufferSpecification& Specification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};

}
