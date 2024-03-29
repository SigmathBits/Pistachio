#pragma once

#include "Event.h"

#include "Pistachio/Core/MouseButtonCodes.h"


namespace Pistachio {

	class MouseButtonEvent : public Event {
	public:
		inline PistachioMouseButton MouseButton() const { return m_Button; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << Name() << ": Button=" << m_Button;
			return ss.str();
		}

		EVENT_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_MOUSE_BUTTON | EVENT_CATEGORY_INPUT)

	protected:
		MouseButtonEvent(PistachioMouseButton button)
			: m_Button(button) {}

		PistachioMouseButton m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent {
	public:
		MouseButtonPressedEvent(PistachioMouseButton button)
			: MouseButtonEvent(button) {}

		EVENT_TYPE(MouseButtonPressed)
	};
	
	class MouseButtonReleasedEvent : public MouseButtonEvent {
	public:
		MouseButtonReleasedEvent(PistachioMouseButton button)
			: MouseButtonEvent(button) {}

		EVENT_TYPE(MouseButtonReleased)
	};

	class MouseMovedEvent : public Event {
	public:
		MouseMovedEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y) {}

		inline float X() const { return m_MouseX; }
		inline float Y() const { return m_MouseY; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << Name() << ": X=" << m_MouseX << ", Y=" << m_MouseY;
			return ss.str();
		}

		EVENT_TYPE(MouseMoved)
			EVENT_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT)

	private:
		float m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public Event {
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		inline float XOffset() const { return m_XOffset; }
		inline float YOffset() const { return m_YOffset; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << Name() << ": XOffset=" << m_XOffset << ", YOffset=" << m_YOffset;
			return ss.str();
		}

		EVENT_TYPE(MouseScrolled)
			EVENT_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT)

	private:
		float m_XOffset, m_YOffset;
	};

}
