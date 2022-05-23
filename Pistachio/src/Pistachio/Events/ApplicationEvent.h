#pragma once

#include "Event.h"


namespace Pistachio {

	class PISTACHIO_API WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		inline unsigned int Width() const { return m_Width; }
		inline unsigned int Height() const { return m_Height; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << Name() << ": Width=" << m_Width << ", Height=" << m_Height;
			return ss.str();
		}

		EVENT_TYPE(WindowResize)
		EVENT_CATEGORY(EVENT_CATEGORY_APPLICATION)
	private:
		unsigned int m_Width, m_Height;
	};

	class PISTACHIO_API WindowCloseEvent : public Event {
	public:
		WindowCloseEvent() {}

		EVENT_TYPE(WindowClose)
		EVENT_CATEGORY(EVENT_CATEGORY_APPLICATION)
	};
	
	class PISTACHIO_API AppTickEvent : public Event {
	public:
		AppTickEvent() {}

		EVENT_TYPE(AppTick)
		EVENT_CATEGORY(EVENT_CATEGORY_APPLICATION)
	};

	class PISTACHIO_API AppUpdateEvent : public Event {
	public:
		AppUpdateEvent() {}

		EVENT_TYPE(AppUpdate)
		EVENT_CATEGORY(EVENT_CATEGORY_APPLICATION)
	};

	class PISTACHIO_API AppRenderEvent : public Event {
	public:
		AppRenderEvent() {}

		EVENT_TYPE(AppTick)
		EVENT_CATEGORY(EVENT_CATEGORY_APPLICATION)
	};

}
