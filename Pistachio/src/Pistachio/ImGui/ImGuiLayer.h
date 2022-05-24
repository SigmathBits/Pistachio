#pragma once

#include "Pistachio/Layer.h"

#include "Pistachio/Events/Event.h"
#include "Pistachio/Events/ApplicationEvent.h"
#include "Pistachio/Events/KeyEvent.h"
#include "Pistachio/Events/MouseEvent.h"


namespace Pistachio {

	class PISTACHIO_API ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;

	private:
		bool OnWindowResize(WindowResizeEvent& event) override;
		bool OnWindowFocus(WindowFocusEvent& event) override;
		bool OnWindowLostFocus(WindowLostFocusEvent& event) override;

		bool OnMouseMoved(MouseMovedEvent& event) override;
		bool OnMouseScrolled(MouseScrolledEvent& event) override;
		bool OnMouseButtonPressed(MouseButtonPressedEvent& event) override;
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& event) override;

		bool OnKeyPressed(KeyPressedEvent& event) override;
		bool OnKeyReleased(KeyReleasedEvent& event) override;
		bool OnKeyTyped(KeyTypedEvent& event) override;

	private:
		float m_Time = 0.0f;
	};

}
