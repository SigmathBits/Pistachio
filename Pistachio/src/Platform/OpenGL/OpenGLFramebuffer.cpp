#include "pstpch.h"

#include "OpenGLFramebuffer.h"

#include <glad/glad.h>


namespace Pistachio {

	namespace Utils {

		static GLenum TextureTarget(bool multisampled) {
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, unsigned int* outIDs, size_t count) {
			glCreateTextures(TextureTarget(multisampled), (GLsizei)count, outIDs);
		}

		static void BindTexture(bool multisampled, unsigned int ID) {
			glBindTexture(TextureTarget(multisampled), ID);
		}

		static void AttachColourTexture(unsigned int ID, int samples, GLenum internalFormat, GLenum format, unsigned int width, unsigned int height, int index) {
			bool multisampled = samples > 1;
			if (multisampled) {
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			} else {
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), ID, 0);
		}

		static void AttachDepthTexture(unsigned int ID, int samples, GLenum internalFormat, GLenum attachmentType, unsigned int width, unsigned int height) {
			bool multisampled = samples > 1;
			if (multisampled) {
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			} else {
				glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), ID, 0);
		}

		static GLenum FramebufferTextureFormatToOpenGLFormat(FramebufferTextureFormat textureFormat) {
			switch (textureFormat) {
				case FramebufferTextureFormat::RGBA8: {
					return GL_RGBA8;
					break;
				}
				case FramebufferTextureFormat::RED_INTEGER: {
					return GL_RED_INTEGER;
					break;
				}
				default: {
					PST_CORE_ASSERT(false, "Unimplimented Texuture Format");
					return 0;
					break;
				}
			}
		}

		static bool IsDepthFormat(FramebufferTextureFormat textureFormat) {
			switch (textureFormat) {
				case FramebufferTextureFormat::DEPTH24STENCIL8:
					return true;
				default:
					return false;
			}
		}

	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec) 
		: m_Specification(spec) {

		for (auto attachment : m_Specification.AttachmentsSpecification.Attachments) {
			if (Utils::IsDepthFormat(attachment.TextureFormat)) {
				m_DepthAttachmentSpecification = attachment.TextureFormat;
			} else {
				m_ColourAttachmentSpecifications.emplace_back(attachment.TextureFormat);
			}
		}

		Revalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer() {
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures((GLsizei)m_ColourAttachmentRendererIDs.size(), m_ColourAttachmentRendererIDs.data());
		glDeleteTextures(1, &m_DepthAttachmentRendererID);
	}

	void OpenGLFramebuffer::Revalidate() {
		if (m_RendererID != 0) {
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures((GLsizei)m_ColourAttachmentRendererIDs.size(), m_ColourAttachmentRendererIDs.data());
			glDeleteTextures(1, &m_DepthAttachmentRendererID);

			m_ColourAttachmentRendererIDs.clear();
			m_DepthAttachmentRendererID = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisampled = m_Specification.Samples > 1;

		// Attachments
		if (m_ColourAttachmentSpecifications.size()) {
			m_ColourAttachmentRendererIDs.resize(m_ColourAttachmentSpecifications.size());

			Utils::CreateTextures(multisampled, m_ColourAttachmentRendererIDs.data(), m_ColourAttachmentRendererIDs.size());

			for (size_t i = 0; i < m_ColourAttachmentRendererIDs.size(); i++) {
				Utils::BindTexture(multisampled, m_ColourAttachmentRendererIDs[i]);

				switch (m_ColourAttachmentSpecifications[i].TextureFormat) {
					case FramebufferTextureFormat::RGBA8: {
						Utils::AttachColourTexture(
							m_ColourAttachmentRendererIDs[i], m_Specification.Samples, 
							GL_RGBA8, GL_RGBA,
							m_Specification.Width, m_Specification.Height, (int)i
						);
						break;
					}
					case FramebufferTextureFormat::RED_INTEGER: {
						Utils::AttachColourTexture(
							m_ColourAttachmentRendererIDs[i], m_Specification.Samples, 
							GL_R32I, GL_RED_INTEGER,
							m_Specification.Width, m_Specification.Height, (int)i
						);
						break;
					}
				}
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None) {
			Utils::CreateTextures(multisampled, &m_DepthAttachmentRendererID, 1);
			Utils::BindTexture(multisampled, m_DepthAttachmentRendererID);

			switch (m_DepthAttachmentSpecification.TextureFormat) {
				case FramebufferTextureFormat::DEPTH24STENCIL8: {
					Utils::AttachDepthTexture(
						m_DepthAttachmentRendererID, m_Specification.Samples, 
						GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT,
						m_Specification.Width, m_Specification.Height
					);
					break;
				}
			}
		}

		if (m_ColourAttachmentRendererIDs.size() > 1) {
			PST_CORE_ASSERT(m_ColourAttachmentRendererIDs.size() <= 4, "Can only have a maximum of 4 Colour Attachments");

			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers((GLsizei)m_ColourAttachmentRendererIDs.size(), buffers);
		} else if (m_ColourAttachmentRendererIDs.empty()) {
			// Only Depth Pass
			glDrawBuffer(GL_NONE);
		}

		PST_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::ClearColourAttachment(int attachmentIndex, int value) const {
		PST_CORE_ASSERT(attachmentIndex < m_ColourAttachmentRendererIDs.size(), "Invalid Colour Attachment index");

		auto& spec = m_ColourAttachmentSpecifications[attachmentIndex];

		glClearTexImage(
			m_ColourAttachmentRendererIDs[attachmentIndex], 0,
			Utils::FramebufferTextureFormatToOpenGLFormat(spec.TextureFormat), GL_INT,
			&value
		);
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

	int OpenGLFramebuffer::ReadPixel(unsigned int attachmentIndex, int x, int y) const {
		PST_CORE_ASSERT(attachmentIndex < m_ColourAttachmentRendererIDs.size(), "Invalid Colour Attachment index");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

}
