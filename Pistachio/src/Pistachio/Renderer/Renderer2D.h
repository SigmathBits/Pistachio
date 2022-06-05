#pragma once

#include "Pistachio/Renderer/Camera.h"
#include "Pistachio/Renderer/EditorCamera.h"
#include "Pistachio/Renderer/OrthographicCamera.h"

#include "Pistachio/Renderer/Transform2D.h" 
#include "Pistachio/Renderer/Sprite.h" 


namespace Pistachio {

	class Renderer2D {
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const glm::mat4& projectionViewMatrix);
		static void BeginScene(const Camera& camera, glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const OrthographicCamera& camera);  // TODO: Remove

		static void EndScene();

		static void StartQuadBatch();
		static void StartCircleBatch();
		static void StartLineBatch();

		static void FlushQuadBatch();
		static void FlushCircleBatch();
		static void FlushLineBatch();

		static void NextQuadBatch();
		static void NextCircleBatch();
		static void NextLineBatch();

		// Primitives
		static void DrawQuad(const glm::mat4& transformMatrix, const glm::vec4& colour);
		static void DrawQuad(const Transform2D& transform, const glm::vec4& colour);

		static void DrawSprite(const glm::mat4& transformMatrix, const Sprite& sprite, int entityID = -1);
		static void DrawSprite(const Transform2D& transform, const Sprite& sprite, int entityID = -1);

		static void DrawCircle(const glm::mat4& transformMatrix, const glm::vec4& colour, float thickness = 1.0f, float blur = 0.005f, int entityID = -1);

		static void DrawLine(const glm::vec3& startPosition, const glm::vec3& endPosition, const glm::vec4& colour, int entityID = -1);

		static void DrawRect(const glm::mat4& transformMatrix, const glm::vec4& colour, int entityID = -1);

		static void DrawPolygon(const glm::mat4& transformMatrix, const std::vector<glm::vec3>& points, const glm::vec4& colour, int entityID = -1);


		float LineThickness() const;
		void SetLineThickness(float thickness);

		// Stats
		struct Statistics {
			unsigned int DrawCalls = 0;
			unsigned int QuadCount = 0;

			unsigned int TotalVertexCount() { return 4 * QuadCount; }
			unsigned int TotalIndexCount() { return 6 * QuadCount; }
		};

		static void ResetStats();
		static Statistics RetrieveStats();
	};

}
