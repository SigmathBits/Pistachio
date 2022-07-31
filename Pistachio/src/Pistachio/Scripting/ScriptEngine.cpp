#include "pstpch.h"
#include "ScriptEngine.h"

#include <glm/glm.hpp>

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/attrdefs.h"

#include "Pistachio/Scripting/ScriptGlue.h"


namespace Pistachio {

	namespace Utils {

		// TODO: Move to some FileSystem namespace
		static char* ReadBytes(const std::filesystem::path& filepath, size_t* outSize) {
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream) {
				return nullptr;
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			size_t size = end - stream.tellg();

			if (size == 0) {
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = size;
			return buffer;
		}

		MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath) {
			size_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);

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
	};

	static ScriptEngineData* s_Data;


	enum Accessibility {
		ACCESSIBILITY_NONE      = 0,
		ACCESSIBILITY_PRIVATE   = BIT(0),
		ACCESSIBILITY_INTERNAL  = BIT(1),
		ACCESSIBILITY_PROTECTED = BIT(2),
		ACCESSIBILITY_PUBLIC    = BIT(3),
	};


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

	MonoClass* MonoClassFromName(const std::string& namespaceName, const std::string& className) {
		return mono_class_from_name(s_Data->CoreAssemblyImage, namespaceName.c_str(), className.c_str());
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

	void CallMethod(MonoObject* instance, const char* methodName, void** params = nullptr, size_t paramCount = 0, MonoObject** exception = nullptr) {
		MonoClass* monoClass = mono_object_get_class(instance);

		MonoMethod* method = mono_class_get_method_from_name(monoClass, methodName, paramCount);

		if (method == nullptr) {
			PST_CORE_ERROR("No method called \"{}\" with {} parameters", methodName, paramCount);
			return;
		}

		mono_runtime_invoke(method, instance, params, nullptr);
	}

	MonoClassField* InstanceField(MonoObject* instance, const char* fieldName) {
		MonoClass* monoClass = mono_object_get_class(instance);
		MonoClassField* field = mono_class_get_field_from_name(monoClass, fieldName);

		if (field == nullptr) {
			PST_CORE_ERROR("No field called \"{}\"", fieldName);
			return nullptr;
		}

		return field;
	}

	MonoProperty* InstanceProperty(MonoObject* instance, const char* propertyName) {
		MonoClass* monoClass = mono_object_get_class(instance);
		MonoProperty* monoProperty = mono_class_get_property_from_name(monoClass, propertyName);

		if (monoProperty == nullptr) {
			PST_CORE_ERROR("No property called \"{}\"", propertyName);
			return nullptr;
		}

		return monoProperty;
	}

	void ScriptEngine::Init() {
		s_Data = new ScriptEngineData;

		InitMono();

		LoadAssembly("Resources/Scripts/Pistachio-ScriptCore.dll");


		// Set up internal calls
		ScriptGlue::RegisterInternalFunctions();


		// 1. Create an object (& call constructor)
		s_Data->EntityClass = ScriptClass("Pistachio", "Entity");
		ScriptObject object = s_Data->EntityClass.Instantiate();

		// 2. Call function
		object.CallMethod("PrintMessage");

		// 3. Call function with parameter(s)
		{
			int value = 5;
			void* params[1] = {
				&value,
			};

			object.CallMethod("PrintInt", params, 1);
		}

		{
			int valueA = 6;
			int valueB = 7;
			void* params[2] = {
				&valueA,
				&valueB,
			};

			object.CallMethod("PrintInts", params, 2);
		}

		{
			MonoString* string = mono_string_new(s_Data->AppDomain, "Ponies!");
			object.CallMethod("PrintCustomMessage", (void**)&string, 1);
		}

		// 4. Change properties and fields
		{
			object.CallMethod("PrintFloatVar");

			MonoObject* floatValueObject = object.PropertyValue("FloatVar");
			float floatValue = *(float*)mono_object_unbox(floatValueObject);

			PST_CORE_TRACE("FloatVar = {}", floatValue);

			floatValue += 6.0f;
			void* data[1] = { &floatValue };
			object.SetPropertyValue("FloatVar", &floatValue);

			object.CallMethod("PrintFloatVar");
		}

		{
			object.CallMethod("PrintIntVar");

			int intValue;
			object.FieldValue("IntVar", &intValue);

			PST_CORE_TRACE("IntVar = {}", intValue);

			intValue += 6;
			object.SetFieldValue("IntVar", &intValue);

			object.CallMethod("PrintIntVar");
		}
	}

	void ScriptEngine::Shutdown() {
		ShutdownMono();

		delete s_Data;
		s_Data = nullptr;
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


	ScriptObject::ScriptObject(MonoClass* monoClass)
		: m_MonoClass(monoClass), m_MonoObject(InstantiateClass(m_MonoClass)) {

	}

	void ScriptObject::CallMethod(const std::string& methodName, void** params /*= nullptr*/, size_t paramCount /*= 0*/, MonoObject** exception /*= nullptr*/) {
		MonoMethod* method = mono_class_get_method_from_name(m_MonoClass, methodName.c_str(), paramCount);

		if (method == nullptr) {
			PST_CORE_ERROR("No method called \"{}\" with {} parameters", methodName, paramCount);
			return;
		}

		mono_runtime_invoke(method, m_MonoObject, params, nullptr);
	}

	void Pistachio::ScriptObject::FieldValue(const std::string& fieldName, void* value) {
		MonoClassField* field = mono_class_get_field_from_name(m_MonoClass, fieldName.c_str());

		if (field == nullptr) {
			PST_CORE_ERROR("No field called \"{}\"", fieldName);
			return;
		}

		mono_field_get_value(m_MonoObject, field, value);
	}

	void Pistachio::ScriptObject::SetFieldValue(const std::string& fieldName, void* value) {
		MonoClassField* field = mono_class_get_field_from_name(m_MonoClass, fieldName.c_str());

		if (field == nullptr) {
			PST_CORE_ERROR("No field called \"{}\"", fieldName);
			return;
		}

		mono_field_set_value(m_MonoObject, field, value);
	}

	MonoObject* Pistachio::ScriptObject::PropertyValue(const std::string& propertyName) {
		MonoProperty* monoProperty = mono_class_get_property_from_name(m_MonoClass, propertyName.c_str());

		if (monoProperty == nullptr) {
			PST_CORE_ERROR("No property called \"{}\"", propertyName);
			return nullptr;
		}

		return mono_property_get_value(monoProperty, m_MonoObject, nullptr, nullptr);
	}

	void Pistachio::ScriptObject::SetPropertyValue(const std::string& propertyName, void* value) {
		MonoProperty* monoProperty = mono_class_get_property_from_name(m_MonoClass, propertyName.c_str());

		if (monoProperty == nullptr) {
			PST_CORE_ERROR("No property called \"{}\"", propertyName);
			return;
		}

		mono_property_set_value(monoProperty, m_MonoObject, &value, nullptr);
	}

	ScriptClass::ScriptClass(const std::string& namespaceName, const std::string& className)
		: m_Namespace(namespaceName), m_Name(className) {
		m_MonoClass = MonoClassFromName(namespaceName, className);
	}

	ScriptObject ScriptClass::Instantiate() {
		return ScriptObject(m_MonoClass);
	}

	MonoClassField* ScriptClass::Field(const std::string& fieldName) {
		MonoClassField* field = mono_class_get_field_from_name(m_MonoClass, fieldName.c_str());
	
		if (field == nullptr) {
			PST_CORE_ERROR("No field called \"{}\"", fieldName);
			return nullptr;
		}
	
		return field;
	}
	
	MonoProperty* ScriptClass::Property(const std::string& propertyName) {
		MonoProperty* monoProperty = mono_class_get_property_from_name(m_MonoClass, propertyName.c_str());
	
		if (monoProperty == nullptr) {
			PST_CORE_ERROR("No property called \"{}\"", propertyName);
			return nullptr;
		}
	
		return monoProperty;
	}

}
