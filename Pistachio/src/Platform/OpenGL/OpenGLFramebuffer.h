#pragma once

#include "Pistachio/Renderer/Framebuffer.h"


namespace Pistachio {

	class OpenGLFramebuffer : public Framebuffer {
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Revalidate();

		virtual void ClearColourAttachment(int attachmentIndex, int value) const override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void Resize(unsigned int width, unsigned int height) override;

		virtual unsigned int ColourAttachmentRendererID(unsigned int attachmentIndex = 0) const override { 
			PST_CORE_ASSERT(attachmentIndex < m_ColourAttachmentRendererIDs.size(), "Invalid Colour Attachment index");
			return m_ColourAttachmentRendererIDs[attachmentIndex]; 
		};
		virtual int ReadPixel(unsigned int attachmentIndex, int x, int y) const override;

		virtual const FramebufferSpecification& Specification() const override { return m_Specification; };

	private:
		unsigned int m_RendererID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColourAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<unsigned int> m_ColourAttachmentRendererIDs;
		unsigned int m_DepthAttachmentRendererID;
	};

}
