#pragma once

#include "Pistachio/Renderer/OrthographicCamera.h"


namespace Pistachio {

	class Renderer2D {
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		// Primitives
		static void DrawQuad(const glm::vec2& position, float rotation, const glm::vec2 size, const glm::vec4& colour);
		static void DrawQuad(const glm::vec3& position, float rotation, const glm::vec2 size, const glm::vec4& colour);
	};

}
