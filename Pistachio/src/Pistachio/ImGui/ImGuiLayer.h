#pragma once

#include "Pistachio/Core/Layer.h"

#include "Pistachio/Events/Event.h"
#include "Pistachio/Events/ApplicationEvent.h"
#include "Pistachio/Events/KeyEvent.h"
#include "Pistachio/Events/MouseEvent.h"


namespace Pistachio {

	class ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnImGuiRender() override;

		void Begin();
		void End();

		void AllowMouseEventBlocking(bool block) { m_AllowMouseEventBlocking = block; }
		void AllowKeyboardEventBlocking(bool block) { m_AllowKeyboardEventBlocking = block; }

	private:
		bool OnEvent(Event& event);

		static void ImGuiStylesPsistachio();
		static void ImGuiStyleColoursPistachio();

	private:
		bool m_AllowMouseEventBlocking = true;
		bool m_AllowKeyboardEventBlocking = true;

		float m_Time = 0.0f;
	};

}
