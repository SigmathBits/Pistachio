#pragma once

#include <Pistachio.h>


class Sandbox2DLayer : public Pistachio::Layer {
public:
	Sandbox2DLayer();
	virtual ~Sandbox2DLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Pistachio::Timestep timestep) override;

	virtual void OnImGuiRender() override;

	bool OnEvent(Pistachio::Event& event) override;

private:
	Pistachio::OrthographicCameraController m_CameraController;

	Pistachio::Ref<Pistachio::Texture2D> m_Texture;

	glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
	float m_Rotation = 0.0f;
	glm::vec2 m_Size{1.0f, 1.0f};
	glm::vec4 m_Colour{ 0.486f, 0.686f, 0.255f, 1.0f };
};
