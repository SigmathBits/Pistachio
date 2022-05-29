#include "pstpch.h"

#include "SubTexture2D.h"


namespace Pistachio {

	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize /*= { 1.0f, 1.0f }*/) {
		float spriteTextureWidth = cellSize.x / texture->Width();
		float spriteTextureHeight = cellSize.y / texture->Height();
		
		glm::vec2 min = { coords.x                  * spriteTextureWidth, coords.y                  * spriteTextureHeight };
		glm::vec2 max = { (coords.x + spriteSize.x) * spriteTextureWidth, (coords.y + spriteSize.y) * spriteTextureHeight };

		return CreateRef<SubTexture2D>(texture, min, max);
	}

}
