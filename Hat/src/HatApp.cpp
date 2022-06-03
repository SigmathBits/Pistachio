#include <Pistachio.h>
#include <Pistachio/Core/EntryPoint.h>

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EditorLayer.h"


class Hat : public Pistachio::Application {
public:
	Hat(Pistachio::ApplicationArguments args)
		: Application("Hat", args) {
		PushLayer(new Pistachio::EditorLayer());
	}

	~Hat() override {

	}
};


Pistachio::Application* Pistachio::CreateApplication(ApplicationArguments args) {
	return new Hat(args);
}
