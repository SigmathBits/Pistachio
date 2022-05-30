#include <Pistachio.h>
#include <Pistachio/Core/EntryPoint.h>

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EditorLayer.h"


class Hat : public Pistachio::Application {
public:
	Hat() 
		: Application("Hat") {
		PushLayer(new Pistachio::EditorLayer());
	}

	~Hat() override {

	}
};


Pistachio::Application* Pistachio::CreateApplication() {
	return new Hat();
}
