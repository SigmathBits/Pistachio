#include "pstpch.h"
#include "ScriptEngine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/attrdefs.h"


namespace Pistachio {

	struct ScriptEngineData {
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
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

	static char* ReadBytes(const std::string& filepath, size_t* outSize) {
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

	MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath) {
		size_t fileSize = 0;
		char* fileData = ReadBytes(assemblyPath, &fileSize);

		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, true, &status, false);

		if (status != MONO_IMAGE_OK) {
			const char* errorMessage = mono_image_strerror(status);

			PST_CORE_ASSERT(false, errorMessage);

			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, false);
		mono_image_close(image);

		delete[] fileData;

		return assembly;
	}

	MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className) {
		MonoImage* image = mono_assembly_get_image(s_Data->CoreAssembly);
		MonoClass* monoClass = mono_class_from_name(image, namespaceName, className);

		if (monoClass == nullptr) {
			PST_CORE_ERROR("Class {}.{} not found in assembly", namespaceName, className);
			return nullptr;
		}

		return monoClass;
	}

	MonoObject* InstantiateClass(const char* namespaceName, const char* className) {
		MonoClass* monoClass = GetClassInAssembly(s_Data->CoreAssembly, namespaceName, className);

		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);

		if (instance == nullptr) {
			PST_CORE_ERROR("Failed to instantiate class {}.{}", namespaceName, className);
			return nullptr;
		}

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

	void ScriptEngine::Init() {
		s_Data = new ScriptEngineData;

		InitMono();
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

		s_Data->AppDomain = mono_domain_create_appdomain("PistachioScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);


		// TODO: Move this maybe
		s_Data->CoreAssembly = LoadCSharpAssembly("Resources/Scripts/Pistachio-ScriptCore.dll");
		PrintAssemblyTypes(s_Data->CoreAssembly);


		// 1. Create an object (& call constructor)
		MonoObject* instance = InstantiateClass("Pistachio", "Main");

		// 2. Call function
		CallMethod(instance, "PrintMessage");

		// 3. Call function with parameter(s)
		{
			int value = 5;
			void* params[1] = {
				&value,
			};

			CallMethod(instance, "PrintInt", params, 1);
		}
		
		{
			int valueA = 6;
			int valueB = 7;
			void* params[2] = {
				&valueA,
				&valueB,
			};

			CallMethod(instance, "PrintInts", params, 2);
		}
		
		{
			MonoString* string = mono_string_new(s_Data->AppDomain, "Ponies!");
			CallMethod(instance, "PrintCustomMessage", (void**)&string, 1);
		}

		// 4. Change properties and fields
		{
			CallMethod(instance, "PrintFloatVar");

			MonoProperty* floatVarProperty = InstanceProperty(instance, "FloatVar");

			MonoObject* floatValueObject = mono_property_get_value(floatVarProperty, instance, nullptr, nullptr);
			float floatValue = *(float*)mono_object_unbox(floatValueObject);

			PST_CORE_TRACE("FloatVar = {}", floatValue);

			floatValue += 6.0f;
			void* data[1] = { &floatValue };
			mono_property_set_value(floatVarProperty, instance, data, nullptr);

			CallMethod(instance, "PrintFloatVar");
		}

		{
			CallMethod(instance, "PrintIntVar");

			MonoClassField* intVarField = InstanceField(instance, "IntVar");

			int intValue;
			mono_field_get_value(instance, intVarField, &intValue);

			PST_CORE_TRACE("IntVar = {}", intValue);

			intValue += 6;
			mono_field_set_value(instance, intVarField, &intValue);

			CallMethod(instance, "PrintIntVar");
		}
	}

	void ScriptEngine::ShutdownMono() {
		// TODO: Make shutdown work

		//mono_domain_unload(s_Data->AppDomain);
		//s_Data->AppDomain = nullptr;

		//mono_jit_cleanup(s_Data->RootDomain);
		//s_Data->RootDomain = nullptr;
	}

}
