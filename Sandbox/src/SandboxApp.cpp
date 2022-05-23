#include <Pistachio.h>


class ExampleLayer : public Pistachio::Layer {
public:
	ExampleLayer()
		: Layer("Example") {}

	void OnUpdate() override {
		PST_INFO("ExampleLayer::Update");
	}

	void OnEvent(Pistachio::Event& event) override {
		PST_TRACE(event);
	}
};


class Sandbox : public Pistachio::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {

	}
};


Pistachio::Application* Pistachio::CreateApplication() {
	return new Sandbox();
}
