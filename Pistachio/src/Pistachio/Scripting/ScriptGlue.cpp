#include "pstpch.h"
#include "ScriptGlue.h"

#include <glm/glm.hpp>

#include "mono/metadata/object.h"

#include "Pistachio/Core/LogOverloads.h"


#define PST_ADD_INTERNAL_CALL(function) mono_add_internal_call("Pistachio.InternalCalls::" #function, function)


namespace Pistachio {

	static void NativeLog(MonoString* monoString, int parameter) {
		char* cString = mono_string_to_utf8(monoString);
		std::string string(cString);
		mono_free(cString);

		std::cout << string << ": " << parameter << std::endl;
	}

	static void NativeLog_Vector3(glm::vec3* vector) {
		PST_CORE_WARN("Value: {}", *vector);
	}

	static void Cross(glm::vec3* vectorA, glm::vec3* vectorB, glm::vec3* resultOut) {
		*resultOut = glm::cross(*vectorA, *vectorB);
	}


	void ScriptGlue::RegisterInternalFunctions() {
		PST_ADD_INTERNAL_CALL(NativeLog);
		PST_ADD_INTERNAL_CALL(NativeLog_Vector3);
		PST_ADD_INTERNAL_CALL(Cross);
	}

}