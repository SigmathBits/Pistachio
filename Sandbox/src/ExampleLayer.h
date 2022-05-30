#pragma once

#include <Pistachio.h>


class ExampleLayer : public Pistachio::Layer {
public:
	ExampleLayer();

	void OnAttach() override;

	void OnUpdate(Pistachio::Timestep timestep) override;
	void OnImGuiRender() override;

	bool OnEvent(Pistachio::Event& event) override;

private:
	Pistachio::OrthographicCameraController m_CameraController;

	Pistachio::Ref<Pistachio::VertexArray> m_VertexArray;

	Pistachio::Ref<Pistachio::Texture2D> m_Texture;

	Pistachio::ShaderLibrary m_ShaderLibrary;

	glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Colour{ 0.486f, 0.686f, 0.255f, 1.0f };
};
