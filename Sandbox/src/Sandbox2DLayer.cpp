#include "Sandbox2DLayer.h"

#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"


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

	{
		PST_PROFILE_SCOPE("Renderer Scene 1");

		// Squares
		Pistachio::Renderer2D::BeginScene(m_CameraController.Camera());

		Pistachio::Renderer2D::DrawSprite({ { 0.0f, 0.0f, 0.9f }, { 10.0f, 10.0f } }, { m_CheckerboardTexture, { 0.6f, 0.8f, 0.9f, 0.1f }, 10.0f });

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
		Pistachio::Renderer2D::DrawSprite({ { 0.0f, 0.0f, -0.52f }, glm::radians(rotation) }, { m_PistachioTexture, 2.0f });

		Pistachio::Renderer2D::DrawSprite({ { 0.5f, 0.5f, -0.6f } }, m_RainbowDashTexture);

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
