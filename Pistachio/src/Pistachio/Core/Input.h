#pragma once

#include "Pistachio/Core/KeyCodes.h"
#include "Pistachio/Core/MouseButtonCodes.h"


namespace Pistachio {

	class Input {
	public:
		static bool IsKeyPressed(PistachioKey keyCode);

		static bool IsMouseButtonPressed(PistachioMouseButton button);
		static std::pair<float, float> MousePosition();
		static float MouseX();
		static float MouseY();
	};

}