#pragma once

#include <glm/glm.hpp>

#include "Pistachio/Renderer/Texture.h"


namespace Pistachio {

	struct Sprite {
		Ref<Texture2D>& Texture;
		glm::vec4 TintColour;
		float TilingScale;
		
		Sprite(Ref<Pistachio::Texture2D>& texture, glm::vec4 tintColour = glm::vec4(1.0f), float tilingScale = 1.0f)
			: Texture(texture), TintColour(tintColour), TilingScale(tilingScale) {}

		Sprite(Ref<Pistachio::Texture2D>& texture, float tilingScale)
			: Texture(texture), TintColour(1.0f), TilingScale(tilingScale) {}
	};

}
