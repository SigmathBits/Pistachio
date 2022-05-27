#pragma once

#include "Pistachio/Core/Timestep.h"

#include "Pistachio/Events/Event.h"
#include "Pistachio/Events/ApplicationEvent.h"
#include "Pistachio/Events/KeyEvent.h"
#include "Pistachio/Events/MouseEvent.h"


namespace Pistachio {

	class EventListener {
	public:
		EventListener(EventCategory categories = EVENT_CATEGORY_ALL);
		EventListener(const std::unordered_set<EventType>& events, EventCategory categories = EVENT_CATEGORY_ALL);
		virtual ~EventListener();

		virtual void SendEvent(Event& event);

	protected:
		virtual bool OnEvent(Event& event) { return false; }
		virtual bool OnEventAfter(Event& event) { return false; }

		virtual bool OnWindowResize(WindowResizeEvent& event) { return false; }
		virtual bool OnWindowClose(WindowCloseEvent& event) { return false; }
		virtual bool OnWindowFocus(WindowFocusEvent& event) { return false; }
		virtual bool OnWindowLostFocus(WindowLostFocusEvent& event) { return false; }

		virtual bool OnMouseMoved(MouseMovedEvent& event) { return false; }
		virtual bool OnMouseScrolled(MouseScrolledEvent& event) { return false; }
		virtual bool OnMouseButtonPressed(MouseButtonPressedEvent& event) { return false; }
		virtual bool OnMouseButtonReleased(MouseButtonReleasedEvent& event) { return false; }

		virtual bool OnKeyPressed(KeyPressedEvent& event) { return false; }
		virtual bool OnKeyReleased(KeyReleasedEvent& event) { return false; }
		virtual bool OnKeyTyped(KeyTypedEvent& event) { return false; }

	private:
		void RegisterMatchingEventCallbacks(const std::unordered_set<EventType>& events, EventCategory categories);

	private:
		EventDispatcher m_EventDispatcher;
		EventCategory m_Categories;
	};

}
