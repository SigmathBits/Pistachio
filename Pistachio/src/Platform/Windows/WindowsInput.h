#pragma once

#include "Pistachio/Input.h"

#include "Pistachio/KeyCodes.h"
#include "Pistachio/MouseButtonCodes.h"


namespace Pistachio {

	class WindowsInput : public Input {
	protected:
		virtual bool IsKeyPressedImpl(PistachioKey keyCode) override;

		virtual bool IsMouseButtonPressedImpl(PistachioMouseButton button) override;
		virtual std::pair<float, float> MousePositionImpl() override;
		virtual float MouseXImpl() override;
		virtual float MouseYImpl() override;
	};

}
