#include "pstpch.h"
#include "Box2DUtils.h"

#include "Pistachio/Core/Log.h"


namespace Pistachio {

	namespace Utils {

		b2BodyType RigidBody2DTypeToBox2DType(RigidBody2DComponent::BodyType bodyType) {
			switch (bodyType) {
				case RigidBody2DComponent::BodyType::Static:
					return b2_staticBody;
					break;
				case RigidBody2DComponent::BodyType::Dynamic:
					return b2_dynamicBody;
					break;
				case RigidBody2DComponent::BodyType::Kinematic:
					return b2_kinematicBody;
					break;
				default:
					break;
			}

			PST_CORE_ASSERT(false, "Unrecognised body type");
			return b2_staticBody;
		}

	}
}