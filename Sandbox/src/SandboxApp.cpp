#include <Pistachio.h>

#include "imgui/imgui.h"


class ExampleLayer : public Pistachio::Layer {
public:
	ExampleLayer()
		: Layer("Example") {}

	void OnUpdate() override {
		if (Pistachio::Input::IsKeyPressed(PST_KEY_TAB)) {
			PST_TRACE("Tab key is pressed! (polled)");
		}
	}

	void OnImGuiRender() override {
		ImGui::Begin("Greetings, Equestria!");
		ImGui::Text("Ponies!");
		ImGui::End();
	}

	bool OnKeyPressed(Pistachio::KeyPressedEvent& event) override {
		if (event.KeyCode() == PST_KEY_S) {
			PST_TRACE("S key pressed!");
		}
		return false;
	}
};


class Sandbox : public Pistachio::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}

	~Sandbox() override {

	}
};


Pistachio::Application* Pistachio::CreateApplication() {
	return new Sandbox();
}
