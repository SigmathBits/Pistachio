#pragma once

#include <box2d/b2_body.h>

#include "Pistachio/Scene/Components.h"


namespace Pistachio {

	namespace Utils {

		b2BodyType RigidBody2DTypeToBox2DType(RigidBody2DComponent::BodyType bodyType);

	}

}
