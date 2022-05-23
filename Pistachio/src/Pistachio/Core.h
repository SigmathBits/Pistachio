#pragma once


#define BIT(x) (1 << x)

#define PST_BIND_EVENT_FUNCTION(function) std::bind(&function, this, std::placeholders::_1)


#ifdef PST_PLATFORM_WINDOWS
	#ifdef PST_BUILD_DLL
		#define PISTACHIO_API __declspec(dllexport)
	#else
		#define PISTACHIO_API __declspec(dllimport)
	#endif
#else
	#error Pistachio only supports Windows!
#endif

#ifdef PST_ENABLE_ASSERTS
	#define PST_CORE_ASSERT(x, ...) { if(!(x)) { PST_CORE_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define PST_ASSERT(x, ...)      { if(!(x)) { PST_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define PST_CORE_ASSERT(x, ...) 
	#define PST_ASSERT(x, ...)      
#endif
