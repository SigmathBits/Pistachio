#pragma once

#include "Pistachio/Renderer/Texture.h"


namespace Pistachio {

	class OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(const std::string& filepath);
		virtual ~OpenGLTexture2D();

		virtual inline unsigned int Width() const override { return m_Width; }
		virtual inline unsigned int Height() const override { return m_Height; }

		virtual void Bind(unsigned int slot = 0) const override;

	private:
		unsigned int m_RendererID;
		std::string m_Filepath;
		unsigned int m_Width, m_Height;
	};

}
