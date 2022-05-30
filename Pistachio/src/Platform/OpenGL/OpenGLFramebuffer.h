#pragma once

#include "Pistachio/Renderer/Framebuffer.h"


namespace Pistachio {

	class OpenGLFramebuffer : public Framebuffer {
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Revalidate();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void Resize(unsigned int width, unsigned int height) override;

		inline virtual unsigned int ColourAttachmentRendererID() const override { return m_ColourAttachmentRendererID; };

		virtual const FramebufferSpecification& Specification() const override { return m_Specification; };

	private:
		unsigned int m_RendererID = 0;
		unsigned int m_ColourAttachmentRendererID = 0;
		unsigned int m_DepthAttachmentRendererID = 0;
		FramebufferSpecification m_Specification;
	};

}
