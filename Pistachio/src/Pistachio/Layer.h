#pragma once

#include "Pistachio/Core.h"

#include "Pistachio/Events/Event.h"
#include "Pistachio/Events/ApplicationEvent.h"
#include "Pistachio/Events/KeyEvent.h"
#include "Pistachio/Events/MouseEvent.h"


namespace Pistachio {

	class PISTACHIO_API Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}

		virtual void OnImGuiRender() {}

		virtual void OnEvent(Event& event);

		inline const std::string& Name() const { return m_DebugName; }

	protected:
		virtual bool OnWindowResize(WindowResizeEvent& event) { return false; }
		virtual bool OnWindowFocus(WindowFocusEvent& event) { return false; }
		virtual bool OnWindowLostFocus(WindowLostFocusEvent& event) { return false; }

		virtual bool OnMouseMoved(MouseMovedEvent& event) { return false; }
		virtual bool OnMouseScrolled(MouseScrolledEvent& event) { return false; }
		virtual bool OnMouseButtonPressed(MouseButtonPressedEvent& event) { return false; }
		virtual bool OnMouseButtonReleased(MouseButtonReleasedEvent& event) { return false; }

		virtual bool OnKeyPressed(KeyPressedEvent& event) { return false; }
		virtual bool OnKeyReleased(KeyReleasedEvent& event) { return false; }
		virtual bool OnKeyTyped(KeyTypedEvent& event) { return false; }

	protected:
		std::string m_DebugName;

	private:
		EventDispatcher m_EventDispatcher;
	};

}
