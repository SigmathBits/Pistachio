#pragma once

#include <filesystem>


extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;

	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoProperty MonoProperty;
}


namespace Pistachio {


	class ScriptEngine {
	public:
		static void Init();
		static void Shutdown();

		static void LoadAssembly(const std::filesystem::path& filepath);

	private:
		static void InitMono();
		static void ShutdownMono();
	};


	class ScriptObject {
	public:
		explicit ScriptObject(MonoClass* monoClass);

		void CallMethod(const std::string& methodName, void** params = nullptr, size_t paramCount = 0, MonoObject** exception = nullptr);

		void FieldValue(const std::string& fieldName, void* value);
		void SetFieldValue(const std::string& fieldName, void* value);

		MonoObject* PropertyValue(const std::string& propertyName);
		void SetPropertyValue(const std::string& propertyName, void* value);

		operator MonoObject* () const { return m_MonoObject; }

	private:
		MonoClass* m_MonoClass = nullptr;
		MonoObject* m_MonoObject = nullptr;
	};

	class ScriptClass {
	public:
		ScriptClass() = default;

		ScriptClass(const std::string& namespaceName, const std::string& className);

		ScriptObject Instantiate();

		MonoClassField* Field(const std::string& fieldName);

		MonoProperty* Property(const std::string& propertyName);

		operator MonoClass* () const { return m_MonoClass; }

	private:
		std::string m_Namespace;
		std::string m_Name;

		MonoClass* m_MonoClass = nullptr;
	};

}