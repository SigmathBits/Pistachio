#include <Pistachio.h>
#include <Pistachio/Core/EntryPoint.h>

#include "Sandbox2DLayer.h"


class Sandbox : public Pistachio::Application {
public:
	Sandbox(const Pistachio::ApplicationSpecification& specification) 
		: Application(specification) {
		//PushLayer(new ExampleLayer());

		PushLayer(new Sandbox2DLayer());
	}

	~Sandbox() override {

	}
};


Pistachio::Application* Pistachio::CreateApplication(ApplicationArguments args) {
	Pistachio::ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../Hat";
	spec.Arguments = args;

	return new Sandbox(spec);
}
