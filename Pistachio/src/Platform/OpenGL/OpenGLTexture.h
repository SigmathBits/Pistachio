#pragma once

#include <glad/glad.h>

#include "Pistachio/Renderer/Texture.h"


namespace Pistachio {

	class OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(unsigned int width, unsigned int height);
		OpenGLTexture2D(const std::string& filepath);
		virtual ~OpenGLTexture2D();

		virtual unsigned int RendererID() const override { return m_RendererID; };

		virtual inline unsigned int Width() const override { return m_Width; }
		virtual inline unsigned int Height() const override { return m_Height; }

		virtual void SetData(void* data, size_t size) override;

		virtual void Bind(unsigned int slot = 0) const override;

		virtual std::string ResourceLocation() const { return m_Filepath; };

		virtual bool operator==(const Texture& other) const override { 
			return m_RendererID == ((const OpenGLTexture2D&)other).m_RendererID;
		}

	private:
		unsigned int m_RendererID;
		std::string m_Filepath;
		unsigned int m_Width, m_Height;
		GLenum m_InternalFormat, m_DataFormat;
	};

}
