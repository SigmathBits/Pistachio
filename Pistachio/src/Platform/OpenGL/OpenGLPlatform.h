#pragma once

#include "Pistachio/Platform.h"


namespace Pistachio {

	class OpenGLPlatform : public Platform {
	private:
		virtual float TimeImpl() const override;
	};

}
