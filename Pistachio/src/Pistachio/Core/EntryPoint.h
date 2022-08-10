#pragma once

#include "Pistachio/Core/Application.h"


//#ifdef PST_PLATFORM_WINDOWS

extern Pistachio::Application* Pistachio::CreateApplication(ApplicationArguments args);

int main(int argc, char** argv) {
	Pistachio::Log::Init();
	PST_CORE_INFO("Initialised Log");

	PST_PROFILE_BEGIN_SESSION("Startup", "Pistachio-Profile-Startup.json");
	auto application = Pistachio::CreateApplication({ argc, argv });
	PST_PROFILE_END_SESSION();

	PST_PROFILE_BEGIN_SESSION("Runtime", "Pistachio-Profile-Runtime.json");
	application->Run();
	PST_PROFILE_END_SESSION();

	PST_PROFILE_BEGIN_SESSION("Shutdown", "Pistachio-Profile-Shutdown.json");
	delete application;
	PST_PROFILE_END_SESSION();
}

//#else
//	#error Pistachio only supports Windows!
//#endif
