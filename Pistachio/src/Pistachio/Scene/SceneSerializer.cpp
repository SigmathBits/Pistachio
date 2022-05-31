#include "pstpch.h"

#include "SceneSerializer.h"

#include <yaml-cpp/yaml.h>

#include "Pistachio/Scene/Entity.h"
#include "Pistachio/Scene/Components.h"

namespace YAML {

	template<glm::length_t L, typename T, glm::qualifier Q>
	struct convert<glm::vec<L, T, Q>> {
		static Node encode(const glm::vec<L, T, Q>& vector) {
			Node node;
			for (size_t i = 0; i < vector.length(); i++) {
				node.push_back(vector[i]);
			}
			return node;
		}

		static bool decode(const Node& node, glm::vec<L, T, Q>& vector) {
			if (!node.IsSequence() || node.size() != L) return false;

			for (size_t i = 0; i < vector.length(); i++) {
				vector[i] = node[i].as<float>();
			}

			return true;
		}
	};

}

namespace Pistachio {
	template<glm::length_t L, typename T, glm::qualifier Q>
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec<L, T, Q> vector) {
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (size_t i = 0; i < vector.length(); i++) {
			out << vector[i];
		}
		return out << YAML::EndSeq;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene) 
		: m_Scene(scene) {
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity) {
		out << YAML::BeginMap;  // Entity

		out << YAML::Key << "Entity" << YAML::Value << "1234567890";  // FIXME: Replace with a Entity UUID 

		if (entity.HasComponent<TagComponent>()) {
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;  // TagComponent

			auto& tag = entity.Component<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap;  // TagComponent
		}

		if (entity.HasComponent<TransformComponent>()) {
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;  // TransformComponent

			auto& transformComponent = entity.Component<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << transformComponent.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << transformComponent.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transformComponent.Scale;

			out << YAML::EndMap;  // TransformComponent
		}

		if (entity.HasComponent<CameraComponent>()) {
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;  // CameraComponent

			auto& cameraComponent = entity.Component<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;  // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (unsigned int)camera.CurrentProjectionType();
			out << YAML::Key << "PerspectiveVerticalFOV" << YAML::Value << camera.PerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNearClip" << YAML::Value << camera.PerspectiveNearClip();
			out << YAML::Key << "PerspectiveFarClip" << YAML::Value << camera.PerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.OrthographicSize();
			out << YAML::Key << "OrthographicNearClip" << YAML::Value << camera.OrthographicNearClip();
			out << YAML::Key << "OrthographicFarClip" << YAML::Value << camera.OrthographicFarClip();
			out << YAML::EndMap;  // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap;  // CameraComponent
		}

		if (entity.HasComponent<SpriteRendererComponent>()) {
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;  // SpriteRendererComponent

			auto& spriteComponent = entity.Component<SpriteRendererComponent>();

			out << YAML::Key << "Colour" << YAML::Value << spriteComponent.Sprite.TintColour;
			out << YAML::Key << "TilingScale" << YAML::Value << spriteComponent.Sprite.TilingScale;

			out << YAML::EndMap;  // SpriteRendererComponent
		}

		out << YAML::EndMap;  // Entity
	}

	void SceneSerializer::Serialize(const std::string& filepath) {
		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "Scene" << YAML::Value << "Untitled";

		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->EachEntity([&out](auto entity) {
			SerializeEntity(out, entity);
		});
		out << YAML::EndSeq;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath) {

	}

	bool SceneSerializer::Deserialize(const std::string& filepath) {
		std::ifstream stream(filepath);
		std::stringstream stringStream;
		stringStream << stream.rdbuf();

		YAML::Node data = YAML::Load(stringStream.str());
		if (!data["Scene"]) return false;

		std::string sceneName = data["Scene"].as<std::string>();
		PST_CORE_TRACE("Deserializing scene \"{}\"", sceneName);

		auto entitiesData = data["Entities"];
		if (!entitiesData) return false;

		for (auto entityData : entitiesData) {
			unsigned int uuid = entityData["Entity"].as<unsigned int>();

			std::string name;
			auto tagComponentData = entityData["TagComponent"];
			if (tagComponentData) {
				name = tagComponentData["Tag"].as<std::string>();
			}

			PST_CORE_TRACE("Deserialized entity with ID = {}, name = {}", uuid, name);

			Entity entity = m_Scene->CreateEntity(name);

			auto transformComponentData = entityData["TransformComponent"];
			if (transformComponentData) {
				auto& transformComponent = entity.Component<TransformComponent>();

				transformComponent.Translation = transformComponentData["Translation"].as<glm::vec3>();
				transformComponent.Rotation = transformComponentData["Rotation"].as<glm::vec3>();
				transformComponent.Scale = transformComponentData["Scale"].as<glm::vec3>();
			}

			auto cameraComponentData = entityData["CameraComponent"];
			if (cameraComponentData) {
				auto& cameraComponent = entity.AddComponent<CameraComponent>();
				auto& camera = cameraComponent.Camera;

				auto cameraData = cameraComponentData["Camera"];

				camera.SetProjectionType((SceneCamera::ProjectionType)cameraData["ProjectionType"].as<unsigned int>());

				camera.SetPerspectiveVerticalFOV(cameraData["PerspectiveVerticalFOV"].as<float>());
				camera.SetPerspectiveNearClip(cameraData["PerspectiveNearClip"].as<float>());
				camera.SetPerspectiveFarClip(cameraData["PerspectiveFarClip"].as<float>());

				camera.SetOrthographicSize(cameraData["OrthographicSize"].as<float>());
				camera.SetOrthographicNearClip(cameraData["OrthographicNearClip"].as<float>());
				camera.SetOrthographicFarClip(cameraData["OrthographicFarClip"].as<float>());

				cameraComponent.Primary = cameraComponentData["Primary"].as<bool>();
				cameraComponent.FixedAspectRatio = cameraComponentData["FixedAspectRatio"].as<bool>();
			}

			auto spriteRendererComponentData = entityData["SpriteRendererComponent"];
			if (spriteRendererComponentData) {
				// Default White Texture
				// FIXME: This is a workaround, shouldn't be making textures here
				static auto whiteTexture = Texture2D::Create(1, 1);
				unsigned int whiteTextureData = 0xFFFFFFFF;
				whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

				auto& spriteComponent = entity.AddComponent<SpriteRendererComponent>(whiteTexture);

				spriteComponent.Sprite.TintColour = spriteRendererComponentData["Colour"].as<glm::vec4>();
				spriteComponent.Sprite.TilingScale = spriteRendererComponentData["TilingScale"].as<float>();
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath) {
		PST_CORE_ASSERT(false, "Deserialization Runtime is not yet implemented");
		return false;
	}

}
