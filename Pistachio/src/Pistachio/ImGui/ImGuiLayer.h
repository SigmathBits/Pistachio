#pragma once

#include "Pistachio/Layer.h"

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
		virtual void OnEvent(Event& event) override;

	private:
		bool OnWindowResize(WindowResizeEvent& event);
		bool OnWindowFocus(WindowFocusEvent& event);
		bool OnWindowLostFocus(WindowLostFocusEvent& event);

		bool OnMouseMoved(MouseMovedEvent& event);
		bool OnMouseScrolled(MouseScrolledEvent& event);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& event);

		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnKeyReleased(KeyReleasedEvent& event);
		bool OnKeyTyped(KeyTypedEvent& event);

	private:
		float m_Time = 0.0f;

		static int ImGuiLayer::GLFWKeyToImGuiKey(int key);
	};

}
