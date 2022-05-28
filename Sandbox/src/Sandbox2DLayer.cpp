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
	
	m_Texture = Pistachio::Texture2D::Create("assets/textures/Pistachio.png");
}

void Sandbox2DLayer::OnDetach() {
	PST_PROFILE_FUNCTION();

}

void Sandbox2DLayer::OnUpdate(Pistachio::Timestep timestep) {
	PST_PROFILE_FUNCTION();

	// Render
	{
		PST_PROFILE_SCOPE("Renderer Screen Clear");

		Pistachio::RenderCommand::SetClearColour({ .03f, 0.1f, 0.15f, 1 });
		Pistachio::RenderCommand::Clear();
	}

	{
		PST_PROFILE_SCOPE("Renderer Draw Call");

		// Square
		Pistachio::Renderer2D::BeginScene(m_CameraController.Camera());

		Pistachio::Renderer2D::DrawQuad({ { 0.5f, -0.5f, 0.1f }, 60.0f, { 0.25f, 0.25f } }, { 0.251f, 0.494f, 0.494f, 1.0f });

		Pistachio::Renderer2D::DrawQuad({ { -0.5f, -0.5f, -0.1f }, -60.0f, { 0.25f, 0.25f } }, { 0.251f, 0.494f, 0.494f, 1.0f });

		m_RotatedTransform.Rotation = glm::radians(m_Angle);
		Pistachio::Renderer2D::DrawQuad(m_RotatedTransform, m_Colour);

		Pistachio::Renderer2D::DrawQuad({ { 0.0f, 0.5f, 0.0f } }, m_Texture);

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
}

bool Sandbox2DLayer::OnEvent(Pistachio::Event& event) {
	m_CameraController.SendEvent(event);
	return false;
}
