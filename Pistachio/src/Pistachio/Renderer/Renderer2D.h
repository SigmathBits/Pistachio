#pragma once

#include "Pistachio/Renderer/OrthographicCamera.h"

#include "Pistachio/Renderer/Transform.h" 
#include "Pistachio/Renderer/Sprite.h" 


namespace Pistachio {

	class Renderer2D {
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		// Primitives
		static void DrawQuad(const Transform& transform, const glm::vec4& colour);
		static void DrawQuad(const Transform& transform, const Sprite& sprite);

		static void DrawQuad(const RotatedTransform& transform, const glm::vec4& colour);
		static void DrawQuad(const RotatedTransform& transform, const Sprite& sprite);
	};

}
