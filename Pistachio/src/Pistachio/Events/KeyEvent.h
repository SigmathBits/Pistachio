#pragma once

#include "Event.h"

#include "Pistachio/KeyCodes.h"


namespace Pistachio {

	class PISTACHIO_API KeyEvent : public Event {
	public:
		inline PistachioKey KeyCode() const { return m_KeyCode;  }

		std::string ToString() const override {
			std::stringstream ss;
			ss << Name() << ": KeyCode=" << m_KeyCode;
			return ss.str();
		}

		EVENT_CATEGORY(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT)

	protected:
		KeyEvent(PistachioKey keycode)
			: m_KeyCode(keycode) {}

		PistachioKey m_KeyCode;
	};

	class PISTACHIO_API KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(PistachioKey keycode, unsigned int repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) {}

		inline int RepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << KeyEvent::ToString() << ", RepeatCount=" << m_RepeatCount;
			return ss.str();
		}

		EVENT_TYPE(KeyPressed)

	private:
		unsigned int m_RepeatCount;
	};
	
	class PISTACHIO_API KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(PistachioKey keycode)
			: KeyEvent(keycode) {}

		EVENT_TYPE(KeyReleased)

	};
		
	class PISTACHIO_API KeyTypedEvent : public Event {
	public:
		KeyTypedEvent(char character)
			: m_Character(character) {}

		inline char Character() const { return m_Character; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << Name() << ": Character='" << m_Character << "'";
			return ss.str();
		}

		EVENT_TYPE(KeyTyped)
		EVENT_CATEGORY(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT)

	private:
		char m_Character;
	};

}
