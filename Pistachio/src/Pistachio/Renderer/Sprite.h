#pragma once

#include <glm/glm.hpp>

#include "Pistachio/Renderer/SubTexture2D.h"


namespace Pistachio {

	struct Sprite {
		Ref<SubTexture2D> SubTexture;
		glm::vec4 TintColour;
		float TilingScale;
		
		Sprite(const Ref<Pistachio::SubTexture2D>& texture, const glm::vec4& tintColour = glm::vec4(1.0f), float tilingScale = 1.0f)
			: SubTexture(texture), TintColour(tintColour), TilingScale(tilingScale) {}

		Sprite(const Ref<Pistachio::SubTexture2D>& texture, float tilingScale)
			: SubTexture(texture), TintColour(1.0f), TilingScale(tilingScale) {}
		
		Sprite(const Ref<Pistachio::Texture2D>& texture, const glm::vec4& tintColour = glm::vec4(1.0f), float tilingScale = 1.0f)
			: SubTexture(CreateRef<SubTexture2D>(texture)), TintColour(tintColour), TilingScale(tilingScale) {}

		Sprite(const Ref<Pistachio::Texture2D>& texture, float tilingScale)
			: SubTexture(CreateRef<SubTexture2D>(texture)), TintColour(1.0f), TilingScale(tilingScale) {}
	};

}
