#include "pstpch.h"

#include "Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Pistachio/Renderer/RenderCommand.h"
#include "Pistachio/Renderer/VertexArray.h"
#include "Pistachio/Renderer/Shader.h"
#include "Pistachio/Renderer/UniformBuffer.h"


namespace Pistachio {

	struct QuadVertex {
		glm::vec4 Position;
		glm::vec4 Colour;
		glm::vec2 TextureCoords;
		unsigned int TextureIndex;
		float TilingScale;

		// Editor-Only
		int EntityID = -1;
	};
	
	struct CircleVertex {
		glm::vec4 Position;
		glm::vec3 LocalPosition;
		glm::vec4 Colour;
		float Thickness;
		float Blur;

		// Editor-Only
		int EntityID = -1;
	};

	struct LineVertex {
		glm::vec3 Position;
		glm::vec4 Colour;

		// Editor-Only
		int EntityID = -1;
	};

	struct Renderer2DData {
		static constexpr unsigned int MaxQuads = 10000;
		static constexpr unsigned int MaxVertices = 4 * MaxQuads;
		static constexpr unsigned int MaxIndices = 6 * MaxQuads;
		static constexpr unsigned int MaxTextureSlots = 32;  // TODO: Get from Renderer Capabilities

		static constexpr size_t QuadVertexCount = 4;
		static constexpr glm::vec4 QuadVertexPositions[QuadVertexCount] = {
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f },
		};
		static constexpr glm::vec2 QuadTextureCoords[QuadVertexCount] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },
		};


		// Quads
		unsigned int QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;

		Ref<Shader> QuadShader;

		// Circles
		unsigned int CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		Ref<VertexArray>  CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;

		Ref<Shader> CircleShader;

		float LineThickness = 2.0f;

		// Lines
		unsigned int LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		Ref<VertexArray>  LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;

		Ref<Shader> LineShader;


		Ref<Texture> WhiteTexture;

		std::array<Ref<Texture>, MaxTextureSlots> TextureSlots;
		unsigned int TextureSlotIndex = 1;  // First (0th) texture is WhiteTexture

		Renderer2D::Statistics Stats;

		struct CameraData {
			glm::mat4 ProjectionViewMatrix;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init() {
		PST_PROFILE_FUNCTION();

		/// Rendering objects

		/// Quad Renderer Objects
		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float4, "a_Position" },
			{ ShaderDataType::Float4, "a_Colour" },
			{ ShaderDataType::Float2, "a_TextureCoords" },
			{ ShaderDataType::Int,    "a_TextureIndex" },
			{ ShaderDataType::Float,  "a_TilingScale" },
			// Editor-Only
			{ ShaderDataType::Int,    "a_EntityID" },
		});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		// Index Buffer Indices
		Scoped<unsigned int[]> quadIndices(new unsigned int[s_Data.MaxIndices]);
		
		unsigned int offset = 0;
		for (size_t i = 0; i < s_Data.MaxIndices; i += 6) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;
			
			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;
		
			offset += 4;
		}

		Ref<IndexBuffer> quadIndexBuffer = IndexBuffer::Create(quadIndices.get(), s_Data.MaxIndices);

		s_Data.QuadVertexArray->SetIndexBuffer(quadIndexBuffer);

		s_Data.QuadShader = Shader::Create("assets/shaders/Renderer2D_Quad.glsl");


		/// Circle Renderer Objects
		s_Data.CircleVertexArray = VertexArray::Create();

		s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];

		s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));

		s_Data.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float4, "a_Position" },
			{ ShaderDataType::Float3, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Colour" },
			{ ShaderDataType::Float,  "a_Thickness" },
			{ ShaderDataType::Float,  "a_Blur" },
			// Editor-Only
			{ ShaderDataType::Int,    "a_EntityID" },
			});
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);

		Ref<IndexBuffer> circleIndexBuffer = IndexBuffer::Create(quadIndices.get(), s_Data.MaxIndices);  // Reuse quadIndices for Circle rendering

		s_Data.CircleVertexArray->SetIndexBuffer(circleIndexBuffer);
		
		s_Data.CircleShader = Shader::Create("assets/shaders/Renderer2D_Circle.glsl");


		/// Line Renderer Objects
		s_Data.LineVertexArray = VertexArray::Create();

		s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];

		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));

		s_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Colour" },
			// Editor-Only
			{ ShaderDataType::Int,    "a_EntityID" },
			});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
		
		s_Data.LineShader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");


		/// Default White Texture
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		unsigned int whiteTextureData = 0xFFFFFFFF;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;


		/// Uniform Buffer for Camera Data
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown() {
		PST_PROFILE_FUNCTION();

		delete[] s_Data.QuadVertexBufferBase;
	}

	void Renderer2D::BeginScene(const glm::mat4& projectionViewMatrix) {
		PST_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ProjectionViewMatrix = projectionViewMatrix;
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartQuadBatch();
		StartCircleBatch();
		StartLineBatch();
	}

	void Renderer2D::BeginScene(const Camera& camera, glm::mat4& transform) {
		PST_PROFILE_FUNCTION();

		glm::mat4 projectionViewMatrix = camera.ProjectionMatrix() * glm::inverse(transform);

		BeginScene(projectionViewMatrix);
	}

	void Renderer2D::BeginScene(const EditorCamera& camera) {
		PST_PROFILE_FUNCTION();

		BeginScene(camera.ProjectionViewMatrix());
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera) {
		PST_PROFILE_FUNCTION();

		BeginScene(camera.ProjectionViewMatrix());
	}

	void Renderer2D::EndScene() {
		PST_PROFILE_FUNCTION();

		FlushQuadBatch();
		FlushCircleBatch();
		FlushLineBatch();
	}

	void Renderer2D::StartQuadBatch() {
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::StartCircleBatch() {
		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;
	}

	void Renderer2D::StartLineBatch() {
		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
	}

	void Renderer2D::FlushQuadBatch() {
		PST_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount == 0) return;

		size_t dataSize = (size_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		for (size_t i = 0; i < s_Data.TextureSlotIndex; i++) {
			s_Data.TextureSlots[i]->Bind((unsigned int)i);
		}

		s_Data.QuadShader->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

		s_Data.Stats.DrawCalls++;
	}

	void Renderer2D::FlushCircleBatch() {
		PST_PROFILE_FUNCTION();

		if (s_Data.CircleIndexCount == 0) return;

		size_t dataSize = (size_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);
		s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);

		s_Data.CircleShader->Bind();
		RenderCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);

		s_Data.Stats.DrawCalls++;
	}

	void Renderer2D::FlushLineBatch() {
		PST_PROFILE_FUNCTION();

		if (s_Data.LineVertexCount == 0) return;

		size_t dataSize = (size_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
		s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

		s_Data.LineShader->Bind();
		RenderCommand::SetLineThickness(s_Data.LineThickness);
		RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCount);

		s_Data.Stats.DrawCalls++;
	}

	void Renderer2D::NextQuadBatch() {
		FlushQuadBatch();
		StartQuadBatch();
	}

	void Renderer2D::NextCircleBatch() {
		FlushCircleBatch();
		StartCircleBatch();
	}

	void Renderer2D::NextLineBatch() {
		FlushLineBatch();
		StartLineBatch();
	}

	void Renderer2D::DrawQuad(const glm::mat4& transformMatrix, const glm::vec4& colour) {
		PST_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) {
			NextQuadBatch();
		}

		for (size_t i = 0; i < Renderer2DData::QuadVertexCount; i++) {
			*(s_Data.QuadVertexBufferPtr++) = { transformMatrix * Renderer2DData::QuadVertexPositions[i], colour, Renderer2DData::QuadTextureCoords[i], 0, 1.0f };
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const Transform2D& transform, const glm::vec4& colour) {
		PST_PROFILE_FUNCTION();

		DrawQuad(transform.TransformMatrix(), colour);
	}

	void Renderer2D::DrawSprite(const glm::mat4& transformMatrix, const Sprite& sprite, int entityID /*= -1*/) {
		PST_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) {
			NextQuadBatch();
		}

		unsigned int textureIndex = 0;
		Ref<Texture> texture = sprite.SubTexture->Texture();
		if (!texture) {
			texture = s_Data.WhiteTexture;
		}

		for (size_t i = 1; i < s_Data.TextureSlotIndex; i++) {
			if (*texture == *s_Data.TextureSlots[i]) {
				textureIndex = (unsigned int)i;
				break;
			}
		}

		if (textureIndex == 0) {
			if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots) {
				NextQuadBatch();
			}

			textureIndex = s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex++] = texture;
		}

		glm::vec4 colour = sprite.TintColour;
		const glm::vec2* textureCoords = sprite.SubTexture->TextureCoords();
		float tilingScale = sprite.TilingScale;

		for (size_t i = 0; i < Renderer2DData::QuadVertexCount; i++) {
			*(s_Data.QuadVertexBufferPtr++) = { transformMatrix * Renderer2DData::QuadVertexPositions[i], colour, textureCoords[i], textureIndex, tilingScale, entityID };
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawSprite(const Transform2D& transform, const Sprite& sprite, int entityID /*= -1*/) {
		PST_PROFILE_FUNCTION();

		DrawSprite(transform.TransformMatrix(), sprite, entityID);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transformMatrix, const glm::vec4& colour, float thickness, float blur, int entityID) {
		PST_PROFILE_FUNCTION();

		if (s_Data.CircleIndexCount >= Renderer2DData::MaxIndices) {
			NextCircleBatch();
		}

		for (size_t i = 0; i < Renderer2DData::QuadVertexCount; i++) {
			*(s_Data.CircleVertexBufferPtr++) = { transformMatrix * Renderer2DData::QuadVertexPositions[i], 2.0f * Renderer2DData::QuadVertexPositions[i], colour, thickness, blur, entityID};
		}

		s_Data.CircleIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawLine(const glm::vec3& startPosition, const glm::vec3& endPosition, const glm::vec4& colour, int entityID /*= -1*/) {
		*(s_Data.LineVertexBufferPtr++) = { startPosition, colour, entityID };
		*(s_Data.LineVertexBufferPtr++) = { endPosition, colour, entityID };

		s_Data.LineVertexCount += 2;
	}

	void Renderer2D::DrawRect(const glm::mat4& transformMatrix, const glm::vec4& colour, int entityID /*= -1*/) {
		glm::vec4 lineVertices[Renderer2DData::QuadVertexCount];
		for (size_t i = 0; i < Renderer2DData::QuadVertexCount; i++) {
			lineVertices[i] = transformMatrix * Renderer2DData::QuadVertexPositions[i];
		}

		for (size_t i = 0; i < Renderer2DData::QuadVertexCount; i++) {
			DrawLine(
				lineVertices[i],
				lineVertices[(i + 1) % Renderer2DData::QuadVertexCount],
				colour,
				entityID
			);
		}
	}

	void Renderer2D::DrawPolygon(const glm::mat4& transformMatrix, const std::vector<glm::vec3>& points, const glm::vec4& colour, int entityID) {
		for (size_t i = 0; i < points.size(); i++) {
			DrawLine(
				points[i],
				points[(i + 1) % points.size()],
				colour,
				entityID
			);
		}
	}

	float Renderer2D::LineThickness() const {
		return s_Data.LineThickness;
	}

	void Renderer2D::SetLineThickness(float thickness) {
		s_Data.LineThickness = thickness;
	}

	void Renderer2D::ResetStats() {
		s_Data.Stats = { 0, 0 };
	}

	Renderer2D::Statistics Renderer2D::RetrieveStats() {
		return s_Data.Stats;
	}

}
