#include "pstpch.h"

#include "OpenGLFramebuffer.h"

#include <glad/glad.h>


namespace Pistachio {

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec) 
		: m_Specification(spec) {
		Revalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer() {
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_ColourAttachmentRendererID);
		glDeleteTextures(1, &m_DepthAttachmentRendererID);
	}

	void OpenGLFramebuffer::Revalidate() {
		if (m_RendererID != 0) {
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_ColourAttachmentRendererID);
			glDeleteTextures(1, &m_DepthAttachmentRendererID);
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColourAttachmentRendererID);
		glBindTexture(GL_TEXTURE_2D, m_ColourAttachmentRendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColourAttachmentRendererID, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachmentRendererID);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachmentRendererID);
		//glTextureStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachmentRendererID, 0);

		PST_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::Unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(unsigned int width, unsigned int height) {
		m_Specification.Width = width;
		m_Specification.Height = height;

		Revalidate();
	}

}
