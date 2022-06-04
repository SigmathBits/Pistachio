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
				node.push_back(vector[static_cast<glm::length_t>(i)]);
			}
			return node;
		}

		static bool decode(const Node& node, glm::vec<L, T, Q>& vector) {
			if (!node.IsSequence() || node.size() != L) return false;

			for (size_t i = 0; i < vector.length(); i++) {
				vector[static_cast<glm::length_t>(i)] = node[i].as<float>();
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
			out << vector[static_cast<glm::length_t>(i)];
		}
		return out << YAML::EndSeq;
	}

	static std::string RigidBody2DTypeToString(RigidBody2DComponent::BodyType bodyType) {
		switch (bodyType) {
			case RigidBody2DComponent::BodyType::Static:
				return "Static";
				break;
			case RigidBody2DComponent::BodyType::Dynamic:
				return "Dynamic";
				break;
			case RigidBody2DComponent::BodyType::Kinematic:
				return "Kinematic";
				break;
			default:
				break;
		}

		PST_CORE_ASSERT(false, "Unrecognised body type");
		return "";
	}

	static RigidBody2DComponent::BodyType RigidBody2DTypeFromString(std::string string) {
		if (string == "Static") {
			return RigidBody2DComponent::BodyType::Static;
		} else if (string == "Dynamic") {
			return RigidBody2DComponent::BodyType::Dynamic;
		} else if (string == "Kinematic") {
			return RigidBody2DComponent::BodyType::Kinematic;
		}

		PST_CORE_ASSERT(false, "Unrecognised body type");
		return RigidBody2DComponent::BodyType::Static;
	}


	SceneSerializer::SceneSerializer(const Ref<Scene>& scene) 
		: m_Scene(scene) {
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity) {
		PST_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Entities must have an ID Component");

		out << YAML::BeginMap;  // Entity

		out << YAML::Key << "Entity" << YAML::Value << entity.ID();

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

			std::string textureResourceLocation = "";
			if (auto& texture = spriteComponent.Sprite.SubTexture->Texture()) {
				textureResourceLocation = texture->ResourceLocation();
			}

			out << YAML::Key << "TextureResourceLocation" << YAML::Value << textureResourceLocation;
			out << YAML::Key << "Colour" << YAML::Value << spriteComponent.Sprite.TintColour;
			out << YAML::Key << "TilingScale" << YAML::Value << spriteComponent.Sprite.TilingScale;

			out << YAML::EndMap;  // SpriteRendererComponent
		}

		if (entity.HasComponent<RigidBody2DComponent>()) {
			out << YAML::Key << "RigidBody2DComponent";
			out << YAML::BeginMap;  // RigidBody2DComponent

			auto& rigidBodyComponent = entity.Component<RigidBody2DComponent>();

			out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DTypeToString(rigidBodyComponent.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rigidBodyComponent.FixedRotation;

			out << YAML::EndMap;  // RigidBody2DComponent
		}

		if (entity.HasComponent<BoxCollider2DComponent>()) {
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap;  // BoxCollider2DComponent

			auto& boxColliderComponent = entity.Component<BoxCollider2DComponent>();

			out << YAML::Key << "Offset" << YAML::Value << boxColliderComponent.Offset;
			out << YAML::Key << "Size" << YAML::Value << boxColliderComponent.Size;

			out << YAML::Key << "Density" << YAML::Value << boxColliderComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << boxColliderComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << boxColliderComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << boxColliderComponent.RestitutionThreshold;

			out << YAML::EndMap;  // BoxCollider2DComponent
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
			uint64_t uuid = entityData["Entity"].as<uint64_t>();

			std::string name;
			auto tagComponentData = entityData["TagComponent"];
			if (tagComponentData) {
				name = tagComponentData["Tag"].as<std::string>();
			}

			PST_CORE_TRACE("Deserialized entity with ID = {}, name = {}", uuid, name);

			Entity entity = m_Scene->CreateEntityWithUUID(uuid, name);

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
				auto& spriteComponent = entity.AddComponent<SpriteRendererComponent>();

				if (auto node = spriteRendererComponentData["TextureResourceLocation"]) {
					std::string textureResourceLocation = node.as<std::string>();
					if (!textureResourceLocation.empty()) {
						spriteComponent.Sprite.SubTexture = CreateRef<SubTexture2D>(Texture2D::Create(textureResourceLocation));
					}
				}

				spriteComponent.Sprite.TintColour = spriteRendererComponentData["Colour"].as<glm::vec4>();
				spriteComponent.Sprite.TilingScale = spriteRendererComponentData["TilingScale"].as<float>();
			}

			auto rigidyBodyComponentData = entityData["RigidBody2DComponent"];
			if (rigidyBodyComponentData) {
				auto& rigidBodyComponent = entity.AddComponent<RigidBody2DComponent>();

				rigidBodyComponent.Type = RigidBody2DTypeFromString(rigidyBodyComponentData["BodyType"].as<std::string>());
				rigidBodyComponent.FixedRotation = rigidyBodyComponentData["FixedRotation"].as<bool>();
			}

			auto boxColliderComponentData = entityData["BoxCollider2DComponent"];
			if (boxColliderComponentData) {
				auto& boxColliderComponent = entity.AddComponent<BoxCollider2DComponent>();

				boxColliderComponent.Offset = boxColliderComponentData["Offset"].as<glm::vec2>();
				boxColliderComponent.Size = boxColliderComponentData["Size"].as<glm::vec2>();

				boxColliderComponent.Density = boxColliderComponentData["Density"].as<float>();
				boxColliderComponent.Friction = boxColliderComponentData["Friction"].as<float>();
				boxColliderComponent.Restitution = boxColliderComponentData["Restitution"].as<float>();
				boxColliderComponent.RestitutionThreshold = boxColliderComponentData["RestitutionThreshold"].as<float>();
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath) {
		PST_CORE_ASSERT(false, "Deserialization Runtime is not yet implemented");
		return false;
	}

}
