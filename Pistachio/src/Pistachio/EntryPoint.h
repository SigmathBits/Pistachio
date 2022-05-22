#pragma once


#ifdef PST_PLATFORM_WINDOWS

extern Pistachio::Application* Pistachio::CreateApplication();

int main(int argc, char** argv) {
	auto application = Pistachio::CreateApplication();

	application->Run();

	delete application;
}

#else
	#error Pistachio only supports Windows!
#endif
