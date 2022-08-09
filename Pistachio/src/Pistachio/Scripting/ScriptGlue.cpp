#include "pstpch.h"
#include "ScriptGlue.h"

#include <glm/glm.hpp>

#include <box2d/b2_body.h>

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "Pistachio/Core/Input.h"
#include "Pistachio/Core/LogOverloads.h"

#include "Pistachio/Scripting/ScriptEngine.h"

#include "Pistachio/Utils/Box2DUtils.h"


#define PST_ADD_INTERNAL_CALL(function) mono_add_internal_call("Pistachio.InternalCalls::" #function, function)


namespace Pistachio {

	static std::unordered_map<MonoType*, std::function<void(Entity)>> s_Entity_AddComponentFunctions;
	static std::unordered_map<MonoType*, std::function<void(Entity)>> s_Entity_RemoveComponentFunctions;
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_Entity_HasComponentFunctions;


	inline static Entity EntityFromUUID(UUID entityUUID) {
		Scene* scene = ScriptEngine::SceneContext();
		PST_CORE_ASSERT(scene, "Scene missing");
		Entity entity = scene->EntityByUUID(entityUUID);
		PST_CORE_ASSERT(entity, "Entity not found");
		return entity;
	}

	inline static b2Body* RigidBodyFromEntityUUID(UUID entityUUID) {
		Scene* scene = ScriptEngine::SceneContext();
		PST_CORE_ASSERT(scene, "Scene missing");
		b2Body* body = scene->RigidBodyByUUID(entityUUID);
		PST_CORE_ASSERT(body, "Rigid body not found");
		return body;
	}


	#pragma region Input
	static bool Input_IsKeyDown(PistachioKey keyCode) {
		return Input::IsKeyPressed(keyCode);
	}

	static bool Input_IsMouseButtonDown(PistachioMouseButton button) {
		return Input::IsMouseButtonPressed(button);
	}
	#pragma endregion

	#pragma region Entity Components
	static void Entity_AddComponent(UUID entityUUID, MonoReflectionType* componentType) {
		Entity entity = EntityFromUUID(entityUUID);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		PST_CORE_ASSERT(s_Entity_AddComponentFunctions.find(managedType) != s_Entity_AddComponentFunctions.end(), "Entity has no registered AddComponent<> function");
		
		s_Entity_AddComponentFunctions.at(managedType)(entity);
	}

	static void Entity_RemoveComponent(UUID entityUUID, MonoReflectionType* componentType) {
		Entity entity = EntityFromUUID(entityUUID);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		PST_CORE_ASSERT(s_Entity_AddComponentFunctions.find(managedType) != s_Entity_AddComponentFunctions.end(), "Entity has no registered RemoveComponent<> function");
		
		s_Entity_RemoveComponentFunctions.at(managedType)(entity);
	}

	static bool Entity_HasComponent(UUID entityUUID, MonoReflectionType* componentType) {
		Entity entity = EntityFromUUID(entityUUID);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		PST_CORE_ASSERT(s_Entity_HasComponentFunctions.find(managedType) != s_Entity_HasComponentFunctions.end(), "Entity has no registered HasComponent<> function");

		bool hasEntity = s_Entity_HasComponentFunctions.at(managedType)(entity);

		return hasEntity;
	}
	#pragma endregion

	#pragma region Transform Component
	static void TransformComponent_Translation(UUID entityUUID, glm::vec3* outTranslation) {
		Entity entity = EntityFromUUID(entityUUID);
		*outTranslation = entity.Component<TransformComponent>().Translation;
	}

	static void TransformComponent_SetTranslation(UUID entityUUID, glm::vec3* translation) {
		Entity entity = EntityFromUUID(entityUUID);
		entity.Component<TransformComponent>().Translation = *translation;
	}

	static void TransformComponent_Rotation(UUID entityUUID, glm::vec3* outRotation) {
		Entity entity = EntityFromUUID(entityUUID);
		*outRotation = entity.Component<TransformComponent>().Rotation;
	}

	static void TransformComponent_SetRotation(UUID entityUUID, glm::vec3* rotation) {
		Entity entity = EntityFromUUID(entityUUID);
		entity.Component<TransformComponent>().Rotation = *rotation;
	}

	static void TransformComponent_Scale(UUID entityUUID, glm::vec3* outScale) {
		Entity entity = EntityFromUUID(entityUUID);
		*outScale = entity.Component<TransformComponent>().Scale;
	}

	static void TransformComponent_SetScale(UUID entityUUID, glm::vec3* scale) {
		Entity entity = EntityFromUUID(entityUUID);
		entity.Component<TransformComponent>().Scale = *scale;
	}
	#pragma endregion

	#pragma region Circle Renderer Component
	static void CircleRendererComponent_Colour(UUID entityUUID, glm::vec4* outColour) {
		Entity entity = EntityFromUUID(entityUUID);
		*outColour = entity.Component<CircleRendererComponent>().Colour;
	}

	static void CircleRendererComponent_SetColour(UUID entityUUID, glm::vec4* colour) {
		Entity entity = EntityFromUUID(entityUUID);
		entity.Component<CircleRendererComponent>().Colour = *colour;
	}

	static float CircleRendererComponent_Thickness(UUID entityUUID) {
		Entity entity = EntityFromUUID(entityUUID);
		return entity.Component<CircleRendererComponent>().Thickness;
	}

	static void CircleRendererComponent_SetThickness(UUID entityUUID, float thickness) {
		Entity entity = EntityFromUUID(entityUUID);
		entity.Component<CircleRendererComponent>().Thickness = thickness;
	}

	static float CircleRendererComponent_Blur(UUID entityUUID) {
		Entity entity = EntityFromUUID(entityUUID);
		return entity.Component<CircleRendererComponent>().Blur;
	}

	static void CircleRendererComponent_SetBlur(UUID entityUUID, float blur) {
		Entity entity = EntityFromUUID(entityUUID);
		entity.Component<CircleRendererComponent>().Blur = blur;
	}
	#pragma endregion

	#pragma region Rigid Body 2D Component
	static RigidBody2DComponent::BodyType RigidBody2DComponent_Type(UUID entityUUID) {
		Entity entity = EntityFromUUID(entityUUID);
		return entity.Component<RigidBody2DComponent>().Type;
	}

	static void RigidBody2DComponent_SetType(UUID entityUUID, RigidBody2DComponent::BodyType type) {
		Scene* scene = ScriptEngine::SceneContext();
		PST_CORE_ASSERT(scene, "Scene missing");
		Entity entity = scene->EntityByUUID(entityUUID);
		PST_CORE_ASSERT(entity, "Entity not found");
		b2Body* body = scene->RigidBodyByUUID(entityUUID);
		PST_CORE_ASSERT(body, "Rigid body not found");

		entity.Component<RigidBody2DComponent>().Type = type;
		body->SetType(Utils::RigidBody2DTypeToBox2DType(type));
	}

	static bool RigidBody2DComponent_FixedRotation(UUID entityUUID) {
		Entity entity = EntityFromUUID(entityUUID);
		return entity.Component<RigidBody2DComponent>().FixedRotation;
	}

	static void RigidBody2DComponent_SetFixedRotation(UUID entityUUID, bool fixedRotation) {
		Scene* scene = ScriptEngine::SceneContext();
		PST_CORE_ASSERT(scene, "Scene missing");
		Entity entity = scene->EntityByUUID(entityUUID);
		PST_CORE_ASSERT(entity, "Entity not found");
		b2Body* body = scene->RigidBodyByUUID(entityUUID);
		PST_CORE_ASSERT(body, "Rigid body not found");

		entity.Component<RigidBody2DComponent>().FixedRotation = fixedRotation;
		body->SetFixedRotation(fixedRotation);
	}

	static void RigidBody2DComponent_Position(UUID entityUUID, glm::vec2* outPosition) {
		b2Body* body = RigidBodyFromEntityUUID(entityUUID);
		b2Vec2 position = body->GetPosition();
		*outPosition = { position.x, position.y };
	}

	static void RigidBody2DComponent_SetPosition(UUID entityUUID, glm::vec2* position) {
		b2Body* body = RigidBodyFromEntityUUID(entityUUID);
		body->SetTransform(b2Vec2(position->x, position->y), body->GetAngle());
	}

	static float RigidBody2DComponent_Rotation(UUID entityUUID) {
		b2Body* body = RigidBodyFromEntityUUID(entityUUID);
		return body->GetAngle();
	}

	static void RigidBody2DComponent_SetRotation(UUID entityUUID, float rotation) {
		b2Body* body = RigidBodyFromEntityUUID(entityUUID);
		body->SetTransform(body->GetPosition(), rotation);
	}

	static void RigidBody2DComponent_ApplyLinearImpulse(UUID entityUUID, glm::vec2* impulse, glm::vec2* worldPosition, bool wake) {
		b2Body* body = RigidBodyFromEntityUUID(entityUUID);
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(worldPosition->x, worldPosition->y), wake);
	}

	static void RigidBody2DComponent_ApplyLinearImpulseToCentre(UUID entityUUID, glm::vec2* impulse, bool wake) {
		b2Body* body = RigidBodyFromEntityUUID(entityUUID);
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}
	#pragma endregion


	/* Registering */
	template<typename... C>
	static void RegisterComponents() {
		([]() {
			std::string_view cppName = typeid(C).name();
			size_t colonPos = cppName.find_last_of(':') + 1;
			std::string csName = fmt::format("Pistachio.{}", cppName.substr(colonPos));

			MonoType* managedType = mono_reflection_type_from_name(csName.data(), ScriptEngine::CoreAssemblyImage());
			if (!managedType) {
				PST_CORE_ERROR("Could not find component type \"{}\"", csName);
				return;
			}

			s_Entity_AddComponentFunctions[managedType]    = [](Entity entity) { return entity.AddComponent<C>(); };
			s_Entity_RemoveComponentFunctions[managedType] = [](Entity entity) { return entity.RemoveComponent<C>(); };
			s_Entity_HasComponentFunctions[managedType]    = [](Entity entity) { return entity.HasComponent<C>(); };
		}(), ...);
	}

	template <typename... C>
	static void RegisterComponentGroup(ComponentGroup<C...> componentGroup) {
		RegisterComponents<C...>();
	}

	void ScriptGlue::RegisterAllComponents() {
		RegisterComponentGroup(AllComponents{});
	}

	void ScriptGlue::RegisterInternalFunctions() {
		// Input
		PST_ADD_INTERNAL_CALL(Input_IsKeyDown);
		PST_ADD_INTERNAL_CALL(Input_IsMouseButtonDown);

		// Entity Components
		PST_ADD_INTERNAL_CALL(Entity_HasComponent);
		PST_ADD_INTERNAL_CALL(Entity_AddComponent);
		PST_ADD_INTERNAL_CALL(Entity_RemoveComponent);

		// Circle Renderer Component
		PST_ADD_INTERNAL_CALL(CircleRendererComponent_Colour);
		PST_ADD_INTERNAL_CALL(CircleRendererComponent_SetColour);
		PST_ADD_INTERNAL_CALL(CircleRendererComponent_Thickness);
		PST_ADD_INTERNAL_CALL(CircleRendererComponent_SetThickness);
		PST_ADD_INTERNAL_CALL(CircleRendererComponent_Blur);
		PST_ADD_INTERNAL_CALL(CircleRendererComponent_SetBlur);

		// Transform Component
		PST_ADD_INTERNAL_CALL(TransformComponent_Translation);
		PST_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		PST_ADD_INTERNAL_CALL(TransformComponent_Rotation);
		PST_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		PST_ADD_INTERNAL_CALL(TransformComponent_Scale);
		PST_ADD_INTERNAL_CALL(TransformComponent_SetScale);

		// Rigid Body 2D Component
		PST_ADD_INTERNAL_CALL(RigidBody2DComponent_Type);
		PST_ADD_INTERNAL_CALL(RigidBody2DComponent_SetType);

		PST_ADD_INTERNAL_CALL(RigidBody2DComponent_FixedRotation);
		PST_ADD_INTERNAL_CALL(RigidBody2DComponent_SetFixedRotation);

		PST_ADD_INTERNAL_CALL(RigidBody2DComponent_Position);
		PST_ADD_INTERNAL_CALL(RigidBody2DComponent_SetPosition);
		PST_ADD_INTERNAL_CALL(RigidBody2DComponent_Rotation);
		PST_ADD_INTERNAL_CALL(RigidBody2DComponent_SetRotation);

		PST_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulse);
		PST_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulseToCentre);
	}

}