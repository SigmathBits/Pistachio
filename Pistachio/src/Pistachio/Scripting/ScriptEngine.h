#pragma once

#include <filesystem>

#include "Pistachio/Scene/Entity.h"


extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;

	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoProperty MonoProperty;

	typedef struct _MonoAssembly MonoAssembly;

	typedef struct _MonoImage MonoImage;
}


namespace Pistachio {

	class Scene;


	class ScriptClass {
	public:
		ScriptClass() = default;

		ScriptClass(const std::string& namespaceName, const std::string& className);

		MonoObject* Instantiate();

		MonoMethod* Method(const std::string& methodName, size_t paramCount = 0, MonoObject** exception = nullptr);
		MonoClassField* Field(const std::string& fieldName);
		MonoProperty* Property(const std::string& propertyName);

		operator MonoClass* () const { return m_MonoClass; }

	private:
		std::string m_Namespace;
		std::string m_Name;

		MonoClass* m_MonoClass = nullptr;
	};

	class ScriptObject {
	public:
		explicit ScriptObject(Ref<ScriptClass> scriptClass, Entity entity);

		void InvokeOnCreate();
		void InvokeOnUpdate(float timestep);

		void InvokeMethod(const std::string& methodName, void** params = nullptr, size_t paramCount = 0, MonoObject** exception = nullptr);
		void InvokeMethod(MonoMethod* method, void** params = nullptr, MonoObject** exception = nullptr);

		void FieldValue(const std::string& fieldName, void* value);
		void FieldValue(MonoClassField* field, void* value);
		void SetFieldValue(const std::string& fieldName, void* value);
		void SetFieldValue(MonoClassField* field, void* value);

		MonoObject* PropertyValue(const std::string& propertyName);
		MonoObject* PropertyValue(MonoProperty* monoProperty);
		void SetPropertyValue(const std::string& propertyName, void* value);
		void SetPropertyValue(MonoProperty* monoProperty, void* value);

		operator MonoObject* () const { return m_MonoObject; }

	private:
		Ref<ScriptClass> m_ScriptClass;

		MonoObject* m_MonoObject = nullptr;

		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;
	};


	class ScriptEngine {
	public:
		static void Init();
		static void Shutdown();

		static MonoImage* CoreAssemblyImage();

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();

		static void EntityCreateClassInstance(Entity entity);
		static void EntityOnUpdate(Entity entity, Timestep timestep);

		static bool EntityClassExists(const std::string& fullClassName);

		static void LoadAssembly(const std::filesystem::path& filepath);

		static Scene* SceneContext();
		static std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses();

	private:
		static void InitMono();
		static void ShutdownMono();

		static void LoadAssemblyClasses(MonoAssembly* assembly);
	};

}