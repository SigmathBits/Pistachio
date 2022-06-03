#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Pistachio/Renderer/Sprite.h"

#include "Pistachio/Scene/SceneCamera.h"


namespace Pistachio {

	class ScriptableEntity;

	struct TagComponent {
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent {
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 Transform() const {
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(glm::quat(Rotation));
			transform = glm::scale(transform, Scale);
			return transform;
		}
	};
	
	struct SpriteRendererComponent {
		Pistachio::Sprite Sprite;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const Pistachio::Sprite& sprite)
			: Sprite(sprite) {}
	};

	struct CameraComponent {
		Pistachio::SceneCamera Camera;
		bool Primary = true;  // TODO: Think about moving to Scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct NativeScriptComponent {
		ScriptableEntity* ScriptInstance = nullptr;

		ScriptableEntity* (* CreateScriptInstance)();
		void (* DestroyScriptInstance)(NativeScriptComponent*);

		template<typename T>
		void Bind() {
			CreateScriptInstance = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScriptInstance = [](NativeScriptComponent* scriptComponent) { 
				delete scriptComponent->ScriptInstance; 
				scriptComponent->ScriptInstance = nullptr; 
			};
		}
	};

}
