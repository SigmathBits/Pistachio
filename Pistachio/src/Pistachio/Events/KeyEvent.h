#pragma once

#include <sstream>

#include "Event.h"


namespace Pistachio {

	class PISTACHIO_API KeyEvent : public Event {
	public:
		inline int KeyCode() const { return m_KeyCode;  }

		std::string ToString() const override {
			std::stringstream ss;
			// KeyEvent: KeyCode=10
			ss << Name() << ": KeyCode=" << m_KeyCode;
			return ss.str();
		}

		EVENT_CATEGORY(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT)
	protected:
		KeyEvent(int keycode)
			: m_KeyCode(keycode) {}

		int m_KeyCode;
	};

	class PISTACHIO_API KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(int keycode, int repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) {}

		inline int RepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override {
			std::stringstream ss;
			// KeyPressedEvent: KeyCode=10, RepeatCount=10
			ss << KeyEvent::ToString() << ", RepeatCount=" << m_RepeatCount;
			return ss.str();
		}

		EVENT_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
	};
	
	class PISTACHIO_API KeyReleaseEvent : public KeyEvent {
	public:
		KeyReleaseEvent(int keycode)
			: KeyEvent(keycode) {}

		EVENT_TYPE(KeyPressed)
	};

}
