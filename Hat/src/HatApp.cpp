#include <Pistachio.h>
#include <Pistachio/Core/EntryPoint.h>

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EditorLayer.h"


class Hat : public Pistachio::Application {
public:
	Hat(Pistachio::ApplicationSpecification specification)
		: Application(specification) {
		PushLayer(new Pistachio::EditorLayer());
	}
};


Pistachio::Application* Pistachio::CreateApplication(ApplicationArguments args) {
	ApplicationSpecification spec;
	spec.Name = "Hat";
	spec.Arguments = args;

	return new Hat(spec);
}
