#include "Application.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include "Log.h"


namespace Pistachio {

	Application::Application() {

	}

	Application::~Application() {

	}

	void Application::Run() {
		// Test Event System and Logging
		WindowResizeEvent resizeEvent(1280, 720);
		PST_TRACE(resizeEvent);

		MouseButtonPressedEvent buttonEvent(2);
		PST_INFO(buttonEvent);

		KeyPressedEvent keyEvent(27, 3);

		EventDispatcher eventDispatcher(keyEvent);
		eventDispatcher.Dispatch<KeyPressedEvent>([](KeyPressedEvent& event) {
			PST_TRACE("Served event!");
			PST_TRACE(event);
			return true;
		});
		eventDispatcher.Dispatch<MouseButtonPressedEvent>([](MouseButtonPressedEvent& event) {
			PST_ERROR("Shouldn't see this message!");
			return true;
		});


		for (;;) {

		}
	}

}
