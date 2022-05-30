#pragma once

#include "Pistachio/Renderer/Camera.h"
#include "Pistachio/Renderer/OrthographicCamera.h"

#include "Pistachio/Renderer/Transform.h" 
#include "Pistachio/Renderer/Sprite.h" 


namespace Pistachio {

	class Renderer2D {
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, glm::mat4& transform);
		static void BeginScene(const OrthographicCamera& camera);  // TODO: Remove
		static void EndScene();
		static void Flush();

		// Primitives
		static void DrawQuad(const glm::mat4& transformMatrix, const glm::vec4& colour);
		static void DrawQuad(const Transform& transform, const glm::vec4& colour);
		static void DrawQuad(const RotatedTransform& transform, const glm::vec4& colour);

		static void DrawQuad(const glm::mat4& transformMatrix, const Sprite& sprite);
		static void DrawQuad(const Transform& transform, const Sprite& sprite);
		static void DrawQuad(const RotatedTransform& transform, const Sprite& sprite);

		// Stats
		struct Statistics {
			unsigned int DrawCalls = 0;
			unsigned int QuadCount = 0;

			unsigned int TotalVertexCount() { return 4 * QuadCount; }
			unsigned int TotalIndexCount() { return 6 * QuadCount; }
		};

		static void ResetStats();
		static Statistics RetrieveStats();

	private:
		static void FlushAndReset();
	};

}
