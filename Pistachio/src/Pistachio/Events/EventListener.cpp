#include "pstpch.h"

#include "EventListener.h"

#include "Pistachio/Core/Core.h"


namespace Pistachio {

    EventListener::EventListener(EventCategory categories) 
        : m_Categories(categories) {
        RegisterMatchingEventCallbacks({}, categories);
    }

    EventListener::EventListener(const std::unordered_set<EventType>& events, EventCategory categories /*= EVENT_CATEGORY_ALL*/)
        : m_Categories(categories) {
        RegisterMatchingEventCallbacks(events, categories);
    }

    void EventListener::RegisterMatchingEventCallbacks(const std::unordered_set<EventType>& events, EventCategory categories) {
        PST_PROFILE_FUNCTION();

        /// Set Event Dispatch Callbacks
        // Application Events
        if (events.find(WindowResizeEvent::StaticType()) != events.end() || WindowResizeEvent::StaticCategoryFlags() & categories) {
            m_EventDispatcher.SetEventCallback<WindowResizeEvent>(PST_BIND_EVENT_FUNCTION(OnWindowResize));
        }
        if (events.find(WindowCloseEvent::StaticType()) != events.end() || WindowCloseEvent::StaticCategoryFlags() & categories) {
            m_EventDispatcher.SetEventCallback<WindowCloseEvent>(PST_BIND_EVENT_FUNCTION(OnWindowClose));
        }
        if (events.find(WindowFocusEvent::StaticType()) != events.end() || WindowFocusEvent::StaticCategoryFlags() & categories) {
            m_EventDispatcher.SetEventCallback<WindowFocusEvent>(PST_BIND_EVENT_FUNCTION(OnWindowFocus));
        }
        if (events.find(WindowLostFocusEvent::StaticType()) != events.end() || WindowLostFocusEvent::StaticCategoryFlags() & categories) {
            m_EventDispatcher.SetEventCallback<WindowLostFocusEvent>(PST_BIND_EVENT_FUNCTION(OnWindowLostFocus));
        }

        // Mouse Events
        if (events.find(MouseButtonPressedEvent::StaticType()) != events.end() || MouseButtonPressedEvent::StaticCategoryFlags() & categories) {
            m_EventDispatcher.SetEventCallback<MouseButtonPressedEvent>(PST_BIND_EVENT_FUNCTION(OnMouseButtonPressed));
        }
        if (events.find(MouseButtonReleasedEvent::StaticType()) != events.end() || MouseButtonReleasedEvent::StaticCategoryFlags() & categories) {
            m_EventDispatcher.SetEventCallback<MouseButtonReleasedEvent>(PST_BIND_EVENT_FUNCTION(OnMouseButtonReleased));
        }
        if (events.find(MouseMovedEvent::StaticType()) != events.end() || MouseMovedEvent::StaticCategoryFlags() & categories) {
            m_EventDispatcher.SetEventCallback<MouseMovedEvent>(PST_BIND_EVENT_FUNCTION(OnMouseMoved));
        }
        if (events.find(MouseScrolledEvent::StaticType()) != events.end() || MouseScrolledEvent::StaticCategoryFlags() & categories) {
            m_EventDispatcher.SetEventCallback<MouseScrolledEvent>(PST_BIND_EVENT_FUNCTION(OnMouseScrolled));
        }

        // Key Events
        if (events.find(KeyPressedEvent::StaticType()) != events.end() || KeyPressedEvent::StaticCategoryFlags() & categories) {
            m_EventDispatcher.SetEventCallback<KeyPressedEvent>(PST_BIND_EVENT_FUNCTION(OnKeyPressed));
        }
        if (events.find(KeyReleasedEvent::StaticType()) != events.end() || KeyReleasedEvent::StaticCategoryFlags() & categories) {
            m_EventDispatcher.SetEventCallback<KeyReleasedEvent>(PST_BIND_EVENT_FUNCTION(OnKeyReleased));
        }
        if (events.find(KeyTypedEvent::StaticType()) != events.end() || KeyTypedEvent::StaticCategoryFlags() & categories) {
            m_EventDispatcher.SetEventCallback<KeyTypedEvent>(PST_BIND_EVENT_FUNCTION(OnKeyTyped));
        }
    }

    void EventListener::SendEvent(Event& event) {
        PST_PROFILE_FUNCTION();

        if (!event.m_Handled) {
            event.m_Handled = OnEvent(event);
        }

        m_EventDispatcher.Dispatch(event);

        if (!event.m_Handled) {
            event.m_Handled = OnEventAfter(event);
        }
    }

    EventListener::~EventListener() {

    }

}
