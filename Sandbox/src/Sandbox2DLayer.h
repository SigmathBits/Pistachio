#pragma once

#include <Pistachio.h>


class Sandbox2DLayer : public Pistachio::Layer {
public:
	Sandbox2DLayer();
	virtual ~Sandbox2DLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Pistachio::Timestep timestep) override;

	virtual void OnImGuiRender() override;

	bool OnEvent(Pistachio::Event& event) override;

private:
	Pistachio::OrthographicCameraController m_CameraController;

	Pistachio::Ref<Pistachio::Texture2D> m_PistachioTexture;
	Pistachio::Ref<Pistachio::Texture2D> m_RainbowDashTexture;
	Pistachio::Ref<Pistachio::Texture2D> m_CheckerboardTexture;

	Pistachio::Ref<Pistachio::Texture2D> m_SpriteSheet;
	Pistachio::Ref<Pistachio::SubTexture2D> m_BarrelTexture;

	Pistachio::Transform2D m_RotatedTransform{ { 0.0f, 0.0f, -0.51f }, 0.0f };
	float m_Angle = 0.0f;
	glm::vec4 m_Colour{ 0.486f, 0.686f, 0.255f, 1.0f };
};
