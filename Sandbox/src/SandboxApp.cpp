#include <Pistachio.h>
#include <Pistachio/Core/EntryPoint.h>

#include "Sandbox2DLayer.h"


class Sandbox : public Pistachio::Application {
public:
	Sandbox() {
		//PushLayer(new ExampleLayer());

		PushLayer(new Sandbox2DLayer());
	}

	~Sandbox() override {

	}
};


Pistachio::Application* Pistachio::CreateApplication() {
	return new Sandbox();
}
