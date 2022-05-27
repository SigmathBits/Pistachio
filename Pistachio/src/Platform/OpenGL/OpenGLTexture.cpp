#include "pstpch.h"

#include "OpenGLTexture.h"

#include <glad/glad.h>

#include "stb_image.h"

#include "Pistachio/Core/Core.h"


namespace Pistachio {

	OpenGLTexture2D::OpenGLTexture2D(unsigned int width, unsigned int height) 
		: m_Width(width), m_Height(height), m_InternalFormat(GL_RGBA8), m_DataFormat(GL_RGBA) {
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, width, height);

		// How we scale the texture
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath)
		: m_Filepath(filepath) {
		int width, height, channels;
		// Need to flip our PNG texture as OpenGL expects image data starting from the bottom left (not the top left, like PNG)
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
		PST_CORE_ASSERT(data, "Failed to load image");

		m_Width = width;
		m_Height = height;

		switch (channels) {
			case 4:
				m_InternalFormat = GL_RGBA8;
				m_DataFormat = GL_RGBA;
				break;
			case 3:
				m_InternalFormat = GL_RGB8;
				m_DataFormat = GL_RGB8;
				break;
			default:
				PST_CORE_ASSERT(false, "Format not supported");
				return;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, width, height);

		// How we scale the texture
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, width, height, m_DataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D() {
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, size_t size) {
		unsigned int bytesPerPixel = m_DataFormat == GL_RGBA ? 4 : 3;
		PST_CORE_ASSERT(size == m_Width * m_Height * bytesPerPixel, "Data size must match entire texture dimensions");

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(unsigned int slot) const {
		glBindTextureUnit(slot, m_RendererID);
	}

}
