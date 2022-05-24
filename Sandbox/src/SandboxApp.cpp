#include <Pistachio.h>


class ExampleLayer : public Pistachio::Layer {
public:
	ExampleLayer()
		: Layer("Example") {}

	void OnUpdate() override {
		//PST_TRACE("ExampleLayer::Update");
		if (Pistachio::Input::IsKeyPressed(PST_KEY_TAB)) {
			PST_TRACE("Tab key is pressed! (polled)");
		}
	}

	void OnEvent(Pistachio::Event& event) override {
		Layer::OnEvent(event);

		PST_TRACE(event);
	}

	bool OnKeyPressed(Pistachio::KeyPressedEvent& event) override {
		char c = (char)event.KeyCode();
		if (isprint(c)) {
			PST_TRACE("{0}", c);
		}
		return false;
	}
};


class Sandbox : public Pistachio::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
		PushOverlay(new Pistachio::ImGuiLayer());
	}

	~Sandbox() override {

	}
};


Pistachio::Application* Pistachio::CreateApplication() {
	return new Sandbox();
}
