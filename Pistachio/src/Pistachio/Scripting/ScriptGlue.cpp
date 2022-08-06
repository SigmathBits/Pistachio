#include "pstpch.h"
#include "ScriptGlue.h"

#include <glm/glm.hpp>

#include "mono/metadata/object.h"

#include "Pistachio/Core/Input.h"
#include "Pistachio/Core/LogOverloads.h"

#include "Pistachio/Scripting/ScriptEngine.h"


#define PST_ADD_INTERNAL_CALL(function) mono_add_internal_call("Pistachio.InternalCalls::" #function, function)


namespace Pistachio {

	static bool Input_IsKeyDown(PistachioKey keyCode) {
		return Input::IsKeyPressed(keyCode);
	}

	static bool Input_IsMouseButtonDown(PistachioMouseButton button) {
		return Input::IsMouseButtonPressed(button);
	}

	static void Entity_Translation(UUID entityUUID, glm::vec3* outTranslation) {
		Scene* scene = ScriptEngine::SceneContext();
		Entity entity = scene->EntityByUUID(entityUUID);
		*outTranslation = entity.Component<TransformComponent>().Translation;
	}

	static void Entity_SetTranslation(UUID entityUUID, glm::vec3* translation) {
		Scene* scene = ScriptEngine::SceneContext();
		Entity entity = scene->EntityByUUID(entityUUID);
		entity.Component<TransformComponent>().Translation = *translation;
	}

	static void Entity_Rotation(UUID entityUUID, glm::vec3* outRotation) {
		Scene* scene = ScriptEngine::SceneContext();
		Entity entity = scene->EntityByUUID(entityUUID);
		*outRotation = entity.Component<TransformComponent>().Rotation;
	}

	static void Entity_SetRotation(UUID entityUUID, glm::vec3* rotation) {
		Scene* scene = ScriptEngine::SceneContext();
		Entity entity = scene->EntityByUUID(entityUUID);
		entity.Component<TransformComponent>().Rotation = *rotation;
	}

	static void Entity_Scale(UUID entityUUID, glm::vec3* outScale) {
		Scene* scene = ScriptEngine::SceneContext();
		Entity entity = scene->EntityByUUID(entityUUID);
		*outScale = entity.Component<TransformComponent>().Scale;
	}

	static void Entity_SetScale(UUID entityUUID, glm::vec3* scale) {
		Scene* scene = ScriptEngine::SceneContext();
		Entity entity = scene->EntityByUUID(entityUUID);
		entity.Component<TransformComponent>().Scale = *scale;
	}


	void ScriptGlue::RegisterInternalFunctions() {
		PST_ADD_INTERNAL_CALL(Input_IsKeyDown);
		PST_ADD_INTERNAL_CALL(Input_IsMouseButtonDown);

		PST_ADD_INTERNAL_CALL(Entity_Translation);
		PST_ADD_INTERNAL_CALL(Entity_SetTranslation);
		PST_ADD_INTERNAL_CALL(Entity_Rotation);
		PST_ADD_INTERNAL_CALL(Entity_SetRotation);
		PST_ADD_INTERNAL_CALL(Entity_Scale);
		PST_ADD_INTERNAL_CALL(Entity_SetScale);
	}

}