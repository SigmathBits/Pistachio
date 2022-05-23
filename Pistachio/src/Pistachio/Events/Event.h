#pragma once

#include "Pistachio/Core.h"


namespace Pistachio {

	// All events are blocking
	// In future we may use an event buffer

	enum class EventType {
		None = 0,
		// Application Events
		WindowResize, WindowClose, WindowFocus, WindowLostFocus, /*Not implemented:*/ WindowMoved,
		AppTick, AppUpdate, AppRender,
		// Key Events
		KeyPressed, KeyReleased, KeyTyped,
		// Mouse Events
		MouseMoved, MouseScrolled, MouseButtonPressed, MouseButtonReleased,
	};

	// For easy subscribing based on  the kind of event
	enum EventCategory {
		EVENT_CATEGORY_NONE            = 0,
		EVENT_CATEGORY_APPLICATION     = BIT(0),
		EVENT_CATEGORY_INPUT           = BIT(1),
		EVENT_CATEGORY_KEYBOARD        = BIT(2),
		EVENT_CATEGORY_MOUSE           = BIT(3),
		EVENT_CATEGORY_MOUSE_BUTTON    = BIT(4),
	};


#define EVENT_TYPE(type) static EventType StaticType() { return EventType::##type; } \
	virtual EventType Type() const override { return StaticType(); } \
	virtual const char* Name() const override { return #type "Event"; }

#define EVENT_CATEGORY(category) virtual int CategoryFlags() const override { return category; }


	class PISTACHIO_API Event {
	public:
		bool Handled = false;

		virtual EventType Type() const = 0;
		virtual int CategoryFlags() const = 0;
		virtual const char* Name() const = 0;
		virtual std::string ToString() const { return Name(); }

		inline bool IsInCategory(EventCategory category) const {
			return CategoryFlags() & category;
		}
	};
	
	class EventDispatcher {
		template<typename T>
		using EventFunction = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event) 
			: m_Event(event) {}

		template<typename T>
		bool Dispatch(EventFunction<T> function) {
			if (m_Event.Type() == T::StaticType()) {
				m_Event.Handled = function(*(T*)&m_Event);
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};


	inline std::ostream& operator<<(std::ostream& ostream, const Event& event) {
		return ostream << event.ToString();
	}

}
