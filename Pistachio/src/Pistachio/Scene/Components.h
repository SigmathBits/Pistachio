#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Pistachio/Scene/SceneCamera.h"

#include "Pistachio/Scene/ScriptableEntity.h"


namespace Pistachio {

	struct TagComponent {
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent {
		glm::mat4 Transform{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform) 
			: Transform(transform) {}

		TransformComponent(const glm::vec3& position)
			: Transform(glm::translate(glm::mat4(1.0f), position)) {}

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }
	};
	
	struct SpriteRendererComponent {
		glm::vec4 Colour{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& colour)
			: Colour(colour) {}

		operator glm::vec4& () { return Colour; }
		operator const glm::vec4& () const { return Colour; }
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
