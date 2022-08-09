#include "pstpch.h"
#include "ScriptEngine.h"

#include <glm/glm.hpp>

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/attrdefs.h"

#include "Pistachio/Scripting/ScriptGlue.h"

#include "Pistachio/Utils/FileSystemUtils.h"


namespace Pistachio {

	enum Accessibility {
		ACCESSIBILITY_NONE      = 0,
		ACCESSIBILITY_PRIVATE   = BIT(0),
		ACCESSIBILITY_INTERNAL  = BIT(1),
		ACCESSIBILITY_PROTECTED = BIT(2),
		ACCESSIBILITY_PUBLIC    = BIT(3),
	};


	namespace Utils {

		static Accessibility MonoToPistachioAccessibility(uint32_t accessibilityFlags) {
			switch (accessibilityFlags) {
				case MONO_FIELD_ATTR_PRIVATE:
					return ACCESSIBILITY_PRIVATE;
					break;
				case MONO_FIELD_ATTR_ASSEMBLY:
					return ACCESSIBILITY_INTERNAL;
					break;
				case MONO_FIELD_ATTR_FAMILY:
					return ACCESSIBILITY_PROTECTED;
					break;
				case MONO_FIELD_ATTR_PUBLIC:
					return ACCESSIBILITY_PUBLIC;
					break;

				case MONO_FIELD_ATTR_FAM_AND_ASSEM:
					return (Accessibility)(ACCESSIBILITY_PROTECTED | ACCESSIBILITY_INTERNAL);
					break;
				case MONO_FIELD_ATTR_FAM_OR_ASSEM:
					return (Accessibility)(ACCESSIBILITY_PRIVATE | ACCESSIBILITY_PROTECTED);
					break;

				default:
					return ACCESSIBILITY_NONE;
					break;
			}
		}

		static Accessibility FieldAccessibility(MonoClassField* field) {
			uint32_t accessibilityFlags = mono_field_get_flags(field) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

			return MonoToPistachioAccessibility(accessibilityFlags);
		}

		static Accessibility PropertyAccessibility(MonoProperty* monoProperty) {
			MonoMethod* propertySetter = mono_property_get_set_method(monoProperty);
			if (propertySetter != nullptr) {
				uint32_t accessibilityFlags = mono_method_get_flags(propertySetter, nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;
				if (accessibilityFlags != MONO_FIELD_ATTR_PUBLIC) {
					return ACCESSIBILITY_PRIVATE;
				}
			} else {
				return ACCESSIBILITY_PRIVATE;
			}

			MonoMethod* propertyGetter = mono_property_get_get_method(monoProperty);
			if (propertyGetter != nullptr) {
				uint32_t accessibilityFlags = mono_method_get_flags(propertyGetter, nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;

				return MonoToPistachioAccessibility(accessibilityFlags);
			}

			return ACCESSIBILITY_NONE;
		}

		MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath) {
			size_t fileSize = 0;
			char* fileData = FileSystem::ReadBytes(assemblyPath, &fileSize);

			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, true, &status, false);

			if (status != MONO_IMAGE_OK) {
				const char* errorMessage = mono_image_strerror(status);

				PST_CORE_ASSERT(false, errorMessage);

				return nullptr;
			}

			std::string assemblyPathString = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPathString.c_str(), &status, false);
			mono_image_close(image);

			delete[] fileData;

			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly) {
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int typeCount = mono_table_info_get_rows(typeDefinitionsTable);

			for (size_t i = 0; i < typeCount; i++) {
				uint32_t columns[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, columns, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, columns[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, columns[MONO_TYPEDEF_NAME]);

				PST_CORE_TRACE("{}.{}", nameSpace, name);
			}
		}

	}


	struct ScriptEngineData {
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		ScriptClass EntityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, Ref<ScriptObject>> EntityInstances;

		Scene* SceneContext = nullptr;
	};

	static ScriptEngineData* s_Data;

	MonoClass* MonoClassFromName(const std::string& namespaceName, const std::string& className, MonoImage* image = nullptr) {
		return mono_class_from_name(image ? image : s_Data->CoreAssemblyImage, namespaceName.c_str(), className.c_str());
	}

	MonoObject* InstantiateClass(const std::string& namespaceName, const std::string& className) {
		MonoClass* monoClass = MonoClassFromName(namespaceName, className);

		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);

		if (instance == nullptr) {
			PST_CORE_ERROR("Failed to instantiate class {}.{}", namespaceName, className);
			return nullptr;
		}

		mono_runtime_object_init(instance);

		return instance;
	}

	MonoObject* InstantiateClass(MonoClass* monoClass) {
		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);

		return instance;
	}

	void ScriptEngine::Init() {
		s_Data = new ScriptEngineData;

		InitMono();

		LoadAssembly("Resources/Scripts/Pistachio-ScriptCore.dll");


		// Set up internal calls
		ScriptGlue::RegisterAllComponents();
		ScriptGlue::RegisterInternalFunctions();

		// Get Entity Class
		s_Data->EntityClass = ScriptClass("Pistachio", "Entity");

		LoadAssemblyClasses(s_Data->CoreAssembly);
	}

	void ScriptEngine::Shutdown() {
		ShutdownMono();

		delete s_Data;
		s_Data = nullptr;
	}

	MonoImage* ScriptEngine::CoreAssemblyImage() {
		return s_Data->CoreAssemblyImage;
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene) {
		s_Data->SceneContext = scene;
	}

	void ScriptEngine::OnRuntimeStop() {
		s_Data->SceneContext = nullptr;

		s_Data->EntityInstances.clear();
	}

	void ScriptEngine::EntityCreateClassInstance(Entity entity) {
		const auto& scriptComponent = entity.Component<ScriptComponent>();

		auto entityClass = s_Data->EntityClasses.find(scriptComponent.ClassName);
		if (entityClass == s_Data->EntityClasses.end()) {
			PST_CORE_WARN("The class name \"{}\" does not exist", scriptComponent.ClassName);
			return;
		}

		Ref<ScriptObject> entityInstance = CreateRef<ScriptObject>(entityClass->second, entity);
		s_Data->EntityInstances[entity.UUID()] = entityInstance;
		entityInstance->InvokeOnCreate();
	}

	void ScriptEngine::EntityOnUpdate(Entity entity, Timestep timestep) {
		UUID entityUUID = entity.UUID();

		auto entityInstance = s_Data->EntityInstances.find(entityUUID);
		if (entityInstance == s_Data->EntityInstances.end()) {
			return;
		}

		entityInstance->second->InvokeOnUpdate(timestep);
	}

	bool ScriptEngine::EntityClassExists(const std::string& fullClassName) {
		return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
	}

	void ScriptEngine::InitMono() {
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("PistachioJITRuntime");

		PST_CORE_ASSERT(rootDomain, "Mono Root Domain not initialised");

		s_Data->RootDomain = rootDomain;
	}

	void ScriptEngine::ShutdownMono() {
		// TODO: Make shutdown work

		//mono_domain_unload(s_Data->AppDomain);
		//s_Data->AppDomain = nullptr;

		//mono_jit_cleanup(s_Data->RootDomain);
		//s_Data->RootDomain = nullptr;
	}

	void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath) {
		s_Data->AppDomain = mono_domain_create_appdomain("PistachioScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);

		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);

		//Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
	}

	Scene* ScriptEngine::SceneContext() {
		return s_Data->SceneContext;
	}

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptEngine::EntityClasses() {
		return s_Data->EntityClasses;
	}

	void ScriptEngine::LoadAssemblyClasses(MonoAssembly* assembly) {
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int typeCount = mono_table_info_get_rows(typeDefinitionsTable);

		s_Data->EntityClasses.clear();
		for (size_t i = 0; i < typeCount; i++) {
			uint32_t columns[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, columns, MONO_TYPEDEF_SIZE);

			std::string nameSpace = mono_metadata_string_heap(image, columns[MONO_TYPEDEF_NAMESPACE]);
			std::string name = mono_metadata_string_heap(image, columns[MONO_TYPEDEF_NAME]);
			std::string fullName = nameSpace.empty() ? name : fmt::format("{}.{}", nameSpace, name);

			MonoClass* monoClass = MonoClassFromName(nameSpace, name, image);
			if (monoClass == s_Data->EntityClass) {
				continue;
			}

			bool isEntity = mono_class_is_subclass_of(monoClass, s_Data->EntityClass, false);
			if (isEntity) {
				s_Data->EntityClasses[fullName] = CreateRef<ScriptClass>(nameSpace, name);
			}

			PST_CORE_TRACE("{}.{} (isEntity: {})", nameSpace, name, isEntity);
		}
	}


	ScriptClass::ScriptClass(const std::string& namespaceName, const std::string& className)
		: m_Namespace(namespaceName), m_Name(className) {
		m_MonoClass = MonoClassFromName(namespaceName, className);
	}

	MonoObject* ScriptClass::Instantiate() {
		return InstantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::Method(const std::string& methodName, size_t paramCount, MonoObject** exception) {
		MonoMethod* method = mono_class_get_method_from_name(m_MonoClass, methodName.c_str(), paramCount);

		if (method == nullptr) {
			PST_CORE_WARN("No method called \"{}\" with {} parameters", methodName, paramCount);
			return nullptr;
		}
	}

	MonoClassField* ScriptClass::Field(const std::string& fieldName) {
		MonoClassField* field = mono_class_get_field_from_name(m_MonoClass, fieldName.c_str());

		if (field == nullptr) {
			PST_CORE_WARN("No field called \"{}\"", fieldName);
			return nullptr;
		}

		return field;
	}

	MonoProperty* ScriptClass::Property(const std::string& propertyName) {
		MonoProperty* monoProperty = mono_class_get_property_from_name(m_MonoClass, propertyName.c_str());

		if (monoProperty == nullptr) {
			PST_CORE_WARN("No property called \"{}\"", propertyName);
			return nullptr;
		}

		return monoProperty;
	}

	ScriptObject::ScriptObject(Ref<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass) {
		m_MonoObject = scriptClass->Instantiate();

		m_Constructor = s_Data->EntityClass.Method(".ctor", 1);
		m_OnCreateMethod = scriptClass->Method("OnCreate", 0);
		m_OnUpdateMethod = scriptClass->Method("OnUpdate", 1);

		{
			UUID uuid = entity.UUID();
			void* params[1] = {
				&uuid
			};
			InvokeMethod(m_Constructor, params);
		}
	}

	void ScriptObject::InvokeOnCreate() {
		if (m_OnCreateMethod) {
			InvokeMethod(m_OnCreateMethod);
		}
	}

	void ScriptObject::InvokeOnUpdate(float timestep) {
		if (m_OnUpdateMethod) {
			void* params[1] = {
				&timestep
			};
			InvokeMethod(m_OnUpdateMethod, params);
		}
	}

	void ScriptObject::InvokeMethod(const std::string& methodName, void** params /*= nullptr*/, size_t paramCount /*= 0*/, MonoObject** exception /*= nullptr*/) {
		MonoMethod* method = m_ScriptClass->Method(methodName, paramCount, exception);
		mono_runtime_invoke(method, m_MonoObject, params, nullptr);
	}

	void ScriptObject::InvokeMethod(MonoMethod* method, void** params /*= nullptr*/, MonoObject** exception /*= nullptr*/) {
		mono_runtime_invoke(method, m_MonoObject, params, exception);
	}

	void Pistachio::ScriptObject::FieldValue(const std::string& fieldName, void* value) {
		MonoClassField* field = m_ScriptClass->Field(fieldName);
		mono_field_get_value(m_MonoObject, field, value);
	}

	void Pistachio::ScriptObject::FieldValue(MonoClassField* field, void* value) {
		mono_field_get_value(m_MonoObject, field, value);
	}

	void Pistachio::ScriptObject::SetFieldValue(const std::string& fieldName, void* value) {
		MonoClassField* field = m_ScriptClass->Field(fieldName);
		mono_field_set_value(m_MonoObject, field, value);
	}

	void Pistachio::ScriptObject::SetFieldValue(MonoClassField* field, void* value) {
		mono_field_set_value(m_MonoObject, field, value);
	}

	MonoObject* Pistachio::ScriptObject::PropertyValue(const std::string& propertyName) {
		MonoProperty* monoProperty = m_ScriptClass->Property(propertyName);
		return mono_property_get_value(monoProperty, m_MonoObject, nullptr, nullptr);
	}

	MonoObject* Pistachio::ScriptObject::PropertyValue(MonoProperty* monoProperty) {
		return mono_property_get_value(monoProperty, m_MonoObject, nullptr, nullptr);
	}

	void Pistachio::ScriptObject::SetPropertyValue(const std::string& propertyName, void* value) {
		MonoProperty* monoProperty = m_ScriptClass->Property(propertyName);
		mono_property_set_value(monoProperty, m_MonoObject, &value, nullptr);
	}

	void Pistachio::ScriptObject::SetPropertyValue(MonoProperty* monoProperty, void* value) {
		mono_property_set_value(monoProperty, m_MonoObject, &value, nullptr);
	}

}
