#include "pstpch.h"

#include "Layer.h"


namespace Pistachio {

	Layer::Layer(const std::string& name) 
		: m_DebugName(name) {
        /// Set Event Dispatch Callbacks
        // Application Events
        m_EventDispatcher.SetEventCallback<WindowFocusEvent>(PST_BIND_EVENT_FUNCTION(Layer::OnWindowFocus));
        m_EventDispatcher.SetEventCallback<WindowLostFocusEvent>(PST_BIND_EVENT_FUNCTION(Layer::OnWindowLostFocus));

        // Mouse Events
        m_EventDispatcher.SetEventCallback<MouseMovedEvent>(PST_BIND_EVENT_FUNCTION(Layer::OnMouseMoved));
        m_EventDispatcher.SetEventCallback<MouseScrolledEvent>(PST_BIND_EVENT_FUNCTION(Layer::OnMouseScrolled));
        m_EventDispatcher.SetEventCallback<MouseButtonPressedEvent>(PST_BIND_EVENT_FUNCTION(Layer::OnMouseButtonPressed));
        m_EventDispatcher.SetEventCallback<MouseButtonReleasedEvent>(PST_BIND_EVENT_FUNCTION(Layer::OnMouseButtonReleased));

        // Key Events
        m_EventDispatcher.SetEventCallback<KeyPressedEvent>(PST_BIND_EVENT_FUNCTION(Layer::OnKeyPressed));
        m_EventDispatcher.SetEventCallback<KeyReleasedEvent>(PST_BIND_EVENT_FUNCTION(Layer::OnKeyReleased));
        m_EventDispatcher.SetEventCallback<KeyTypedEvent>(PST_BIND_EVENT_FUNCTION(Layer::OnKeyTyped));
	}

    void Layer::OnEvent(Event& event) {
        m_EventDispatcher.Dispatch(event);
    }

	Layer::~Layer() {

	}

}
