#pragma once

#include "Pistachio/Core/Platform.h"


namespace Pistachio {

	class OpenGLPlatform : public Platform {
	private:
		virtual float TimeImpl() const override;
	};

}
