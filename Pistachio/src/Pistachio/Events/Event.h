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
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
	};

	// For easy subscribing based on  the kind of event
	enum EventCategory {
		EVENT_CATEGORY_NONE            = 0,
		EVENT_CATEGORY_APPLICATION     = BIT(0),
		EVENT_CATEGORY_INPUT           = BIT(1),
		EVENT_CATEGORY_KEYBOARD        = BIT(2),
		EVENT_CATEGORY_MOUSE           = BIT(3),
		EVENT_CATEGORY_MOUSE_BUTTON    = BIT(4),
		EVENT_CATEGORY_ALL             = 0b11111,
	};


#define EVENT_TYPE(type) static EventType StaticType() { return EventType::##type; } \
	virtual EventType Type() const override { return StaticType(); } \
	virtual const char* Name() const override { return #type "Event"; }

#define EVENT_CATEGORY(category) static EventCategory StaticCategoryFlags() { return (EventCategory)(category); } \
	virtual EventCategory CategoryFlags() const override { return (EventCategory)(category); }


	class PISTACHIO_API Event {
	public:
		bool Handled = false;

		virtual EventType Type() const = 0;
		virtual EventCategory CategoryFlags() const = 0;
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
		EventDispatcher() {}

		template<typename T>
		void SetEventCallback(EventFunction<T> callback) {
			m_EventCallbacks[T::StaticType()] = *(EventFunction<Event>*)&callback;
		}

		bool Dispatch(Event& event) const {
			if (event.Handled) return false;

			auto pair = m_EventCallbacks.find(event.Type());
			if (pair == m_EventCallbacks.end()) {
				return false;
			}

			event.Handled = pair->second(event);

			return true;
		}

	private:
		std::unordered_map<EventType, EventFunction<Event>> m_EventCallbacks;
	};


	inline std::ostream& operator<<(std::ostream& ostream, const Event& event) {
		return ostream << event.ToString();
	}

}
