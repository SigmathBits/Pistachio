#pragma once

#include "Pistachio/Scene/Entity.h"


namespace Pistachio {

	class ScriptableEntity {
	public:
		virtual ~ScriptableEntity() = default;

		template<typename T>
		T& Component() {
			return m_Entity.Component<T>();
		}

	protected:
		virtual void OnCreate() {};
		virtual void OnDestroy() {};

		virtual void OnUpdate(Timestep timestep) {};

	private:
		Entity m_Entity;

		friend class Scene;
	};

}
