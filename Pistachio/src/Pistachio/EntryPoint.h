#pragma once


#ifdef PST_PLATFORM_WINDOWS

extern Pistachio::Application* Pistachio::CreateApplication();

int main(int argc, char** argv) {
	Pistachio::Log::Init();
	PST_CORE_INFO("Initialised Log");

	auto application = Pistachio::CreateApplication();

	application->Run();

	delete application;
}

#else
	#error Pistachio only supports Windows!
#endif
