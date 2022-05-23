#include <Pistachio.h>


class ExampleLayer : public Pistachio::Layer {
public:
	ExampleLayer()
		: Layer("Example") {}

	void OnUpdate() override {
		//PST_TRACE("ExampleLayer::Update");
	}

	void OnEvent(Pistachio::Event& event) override {
		PST_TRACE(event);
	}
};


class Sandbox : public Pistachio::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
		PushOverlay(new Pistachio::ImGuiLayer());
	}

	~Sandbox() {

	}
};


Pistachio::Application* Pistachio::CreateApplication() {
	return new Sandbox();
}
