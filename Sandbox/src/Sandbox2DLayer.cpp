#include "Sandbox2DLayer.h"

#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"


static constexpr unsigned int s_MapWidth = 25;
static const char s_MapTiles[] = 
	"                         "
	"     .NNNNNNNNNNNNN,     "
	"    .JOOOOOOOOOOOOOL,    "
	"   .JOOOOOOOOOF7OOOOL,   "
	"  .JOOOOOOOOOOLJOOOOOL,  "
	" .JOOOOOOOOOOOOOOOOOOOL, "
	" COOOOOOOOOOOOOOOOOOOOOD "
	" '7OOOOFUUUUU7OOOOOOOOF` "
	"  '7OOOD     COOOOOOOF`  "
	"   '7OOLNNNNNJOOOOOOF`   "
	"    '7OOOOOOOOOOOOOF`    "
	"     'UUUUUUUUUUUUU`     "
	"                         "
;


Sandbox2DLayer::Sandbox2DLayer()
	: Layer("Sandbox2D", Pistachio::EVENT_CATEGORY_NONE), m_CameraController(1280, 720, true) {

}

Sandbox2DLayer::~Sandbox2DLayer() {

}

void Sandbox2DLayer::OnAttach() {
	PST_PROFILE_FUNCTION();
	
	m_PistachioTexture = Pistachio::Texture2D::Create("assets/textures/Pistachio.png");
	m_RainbowDashTexture = Pistachio::Texture2D::Create("assets/textures/Rainbow-Dash.png");
	m_CheckerboardTexture = Pistachio::Texture2D::Create("assets/textures/Checkerboard.png");

	m_SpriteSheet = Pistachio::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");

	m_MapWidth = s_MapWidth;
	m_MapHeight = strlen(s_MapTiles) / m_MapWidth;

	m_TextureMap['O'] = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {1, 11}, {128, 128});

	m_TextureMap['U'] = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {11, 12}, {128, 128});
	m_TextureMap['N'] = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {11, 10}, {128, 128});

	m_TextureMap['C'] = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {12, 11}, {128, 128});
	m_TextureMap['D'] = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {10, 11}, {128, 128});

	m_TextureMap['J'] = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {12, 10}, {128, 128});
	m_TextureMap['L'] = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {10, 10}, {128, 128});
	m_TextureMap['F'] = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {10, 12}, {128, 128});
	m_TextureMap['7'] = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {12, 12}, {128, 128});

	m_TextureMap['.']  = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {13, 12}, {128, 128});
	m_TextureMap[',']  = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {14, 12}, {128, 128});
	m_TextureMap['\''] = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {13, 11}, {128, 128});
	m_TextureMap['`']  = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {14, 11}, {128, 128});

	m_TextureMap[' '] = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {11, 11}, {128, 128});

	m_BarrelTexture = Pistachio::SubTexture2D::CreateFromCoords(m_SpriteSheet, {8, 2}, {128, 128});

	m_CameraController.SetZoomLevel(4.0f);
}

void Sandbox2DLayer::OnDetach() {
	PST_PROFILE_FUNCTION();

}

void Sandbox2DLayer::OnUpdate(Pistachio::Timestep timestep) {
	PST_PROFILE_FUNCTION();

	{
		PST_PROFILE_SCOPE("CameraController.OnUpdate");

		m_CameraController.OnUpdate(timestep);
	}

	// Render
	Pistachio::Renderer2D::ResetStats();
	{
		PST_PROFILE_SCOPE("Renderer Screen Clear");

		Pistachio::RenderCommand::SetClearColour({ .03f, 0.1f, 0.15f, 1 });
		Pistachio::RenderCommand::Clear();
	}

#if 0
	{
		PST_PROFILE_SCOPE("Renderer Scene 1");

		// Squares
		Pistachio::Renderer2D::BeginScene(m_CameraController.Camera());

		Pistachio::Renderer2D::DrawQuad({ { 0.0f, 0.0f, 0.9f }, { 10.0f, 10.0f } }, { m_CheckerboardTexture, { 0.6f, 0.8f, 0.9f, 0.1f }, 10.0f });

		Pistachio::Renderer2D::DrawQuad({ { 0.5f, 0.5f, 0.2f } }, { 1.0f, 0.0f, 1.0f, 1.0f });

		Pistachio::Renderer2D::DrawQuad({ { -0.5f, -0.5f, 0.2f } }, { 0.0f, 1.0f, 0.0f, 1.0f });

		const float stride = 2.0f;
		for (float y = -stride / 2; y < stride / 2; y += 0.1f) {
			for (float x = -stride / 2; x < stride / 2; x += 0.1f) {
				Pistachio::Renderer2D::DrawQuad({ { x, y, 0.1f }, { 0.09f, 0.09f } },
					{ std::abs(x / stride), std::abs(y / stride), 0.0f, 0.8f });
			}
		}

		Pistachio::Renderer2D::EndScene();
	}

	{
		PST_PROFILE_SCOPE("Renderer Scene 2");

		Pistachio::Renderer2D::BeginScene(m_CameraController.Camera());

		Pistachio::Renderer2D::DrawQuad({ { -0.5f, -0.5f, 0.0f }, glm::radians(10.0f) }, { 0.3f, 0.0f, 0.1f, 1.0f });

		Pistachio::Renderer2D::DrawQuad({ { 0.5f, -0.5f, -0.3f }, glm::radians(60.0f), { 0.25f, 0.25f } }, { 0.251f, 0.494f, 0.494f, 0.8f });

		Pistachio::Renderer2D::DrawQuad({ { -0.5f, -0.5f, -0.31f }, glm::radians(-60.0f),{ 0.25f, 0.25f } }, { .251f, 0.494f, 0.494f, 0.5f });

		m_RotatedTransform.Rotation = glm::radians(m_Angle);
		Pistachio::Renderer2D::DrawQuad(m_RotatedTransform, m_Colour);

		static float rotation = 0.0f;
		rotation += 30.0f * timestep;
		Pistachio::Renderer2D::DrawQuad({ { 0.0f, 0.0f, -0.52f }, glm::radians(rotation) }, { m_PistachioTexture, 2.0f });

		Pistachio::Renderer2D::DrawQuad({ { 0.5f, 0.5f, -0.6f } }, m_RainbowDashTexture);

		Pistachio::Renderer2D::EndScene();
	}
#endif

	{
		PST_PROFILE_SCOPE("Renderer Scene 3");

		Pistachio::Renderer2D::BeginScene(m_CameraController.Camera());

		for (size_t y = 0; y < m_MapHeight; y++) {
			for (size_t x = 0; x < m_MapWidth; x++) {
				char tile = s_MapTiles[m_MapWidth * y + x];

				Pistachio::Ref<Pistachio::SubTexture2D> subtexture;

				if (m_TextureMap.find(tile) == m_TextureMap.end()) {
					subtexture = m_BarrelTexture;
				} else {
					 subtexture = m_TextureMap[tile];
				}

				Pistachio::Renderer2D::DrawQuad({ {(float)x - (float)m_MapWidth / 2.0f, (float)m_MapHeight / 2.0f - (float)y, 0.0f}}, subtexture);
			}
		}

		Pistachio::Renderer2D::EndScene();
	}
}

void Sandbox2DLayer::OnImGuiRender() {
	PST_PROFILE_FUNCTION();

	ImGui::Begin("Quad");
	ImGui::DragFloat3("Position", glm::value_ptr(m_RotatedTransform.Position), 0.005f);
	ImGui::DragFloat("Angle", &m_Angle, 1.0f, 0.0f, 360.0f);
	ImGui::DragFloat2("Size", glm::value_ptr(m_RotatedTransform.Size), 0.01f, 0.01f, 10.0f);
	ImGui::ColorEdit4("Colour", glm::value_ptr(m_Colour));
	ImGui::End();

	auto stats = Pistachio::Renderer2D::RetrieveStats();
	ImGui::Begin("Stats");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quad Count: %d", stats.QuadCount);
	ImGui::Text("Vertex Count: %d", stats.TotalVertexCount());
	ImGui::Text("Index Count: %d", stats.TotalIndexCount());
	ImGui::End();
}

bool Sandbox2DLayer::OnEvent(Pistachio::Event& event) {
	m_CameraController.SendEvent(event);
	return false;
}
