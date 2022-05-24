#pragma once

#include "Pistachio/Core.h"

#include "Pistachio/KeyCodes.h"
#include "Pistachio/MouseButtonCodes.h"


namespace Pistachio {

	class PISTACHIO_API Input {
	public:
		inline static bool IsKeyPressed(PistachioKey keyCode) { return s_Instance->IsKeyPressedImpl(keyCode); }

		inline static bool IsMouseButtonPressed(PistachioMouseButton button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		inline static std::pair<float, float> MousePosition() { return s_Instance->MousePositionImpl(); }
		inline static float MouseX() { return s_Instance->MouseXImpl(); }
		inline static float MouseY() { return s_Instance->MouseYImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(PistachioKey keyCode) = 0;

		virtual bool IsMouseButtonPressedImpl(PistachioMouseButton button) = 0;
		virtual std::pair<float, float> MousePositionImpl() = 0;
		virtual float MouseXImpl() = 0;
		virtual float MouseYImpl() = 0;

	private:
		static Input* s_Instance;
	};

}