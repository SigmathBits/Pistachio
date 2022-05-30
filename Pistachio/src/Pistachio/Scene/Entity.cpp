#include "pstpch.h"

#include "Entity.h"


namespace Pistachio {

	Entity::Entity(entt::entity entityHandle, Scene* scene) 
		: m_EntityHandle(entityHandle), m_Scene(scene) {

	}

}
