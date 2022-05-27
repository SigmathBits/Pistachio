#include "Sandbox2DLayer.h"

#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"


Sandbox2DLayer::Sandbox2DLayer()
	: Layer("Sandbox2D", Pistachio::EVENT_CATEGORY_NONE), m_CameraController(1280, 720, true) {

}

void Sandbox2DLayer::OnAttach() {

}

void Sandbox2DLayer::OnDetach() {
}

void Sandbox2DLayer::OnUpdate(Pistachio::Timestep timestep) {
	// Render
	Pistachio::RenderCommand::SetClearColour({ .03f, 0.1f, 0.15f, 1 });
	Pistachio::RenderCommand::Clear();

	{
		// Square
		Pistachio::Renderer2D::BeginScene(m_CameraController.Camera());

		Pistachio::Renderer2D::DrawQuad(m_Position, m_Rotation, m_Size, m_Colour);

		Pistachio::Renderer2D::EndScene();
	}
}

void Sandbox2DLayer::OnImGuiRender() {
	ImGui::Begin("Quad");
	ImGui::DragFloat2("Position", glm::value_ptr(m_Position), 0.005f);
	ImGui::DragFloat("Rotation", &m_Rotation, 1.0f, 0.0f, 360.0f);
	ImGui::DragFloat2("Size", glm::value_ptr(m_Size), 0.01f, 0.01f, 10.0f);
	ImGui::ColorEdit4("Colour", glm::value_ptr(m_Colour));
	ImGui::End();
}

bool Sandbox2DLayer::OnEvent(Pistachio::Event& event) {
	m_CameraController.SendEvent(event);
	return false;
}
