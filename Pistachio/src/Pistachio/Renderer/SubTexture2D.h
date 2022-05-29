#pragma once

#include <glm/glm.hpp>

#include "Texture.h"


namespace Pistachio {

	class SubTexture2D {
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max)
			: m_Texture(texture),
			m_TextureCoords{
				{ min.x, min.y },
				{ max.x, min.y },
				{ max.x, max.y },
				{ min.x, max.y },
			} {}

		SubTexture2D(const Ref<Texture2D>& texture) : m_Texture(texture), 
			m_TextureCoords{
				{0.0f, 0.0f},
				{1.0f, 0.0f},
				{1.0f, 1.0f},
				{0.0f, 1.0f},
			} {}

		const Ref<Texture2D> Texture() const { return m_Texture; }
		const glm::vec2* TextureCoords() { return m_TextureCoords; }

		static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = { 1.0f, 1.0f });

	private:
		Ref<Texture2D> m_Texture;

		glm::vec2 m_TextureCoords[4];
	};

}