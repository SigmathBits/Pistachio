#pragma once

#include "Pistachio/Layer.h"

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

	private:
		static void ImGuiStylesPsistachio();
		static void ImGuiStyleColoursPistachio();

	private:
		float m_Time = 0.0f;
	};

}
