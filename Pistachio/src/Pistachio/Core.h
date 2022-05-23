#pragma once


#define BIT(x) (1 << x)


#ifdef PST_PLATFORM_WINDOWS
	#ifdef PST_BUILD_DLL
		#define PISTACHIO_API __declspec(dllexport)
	#else
		#define PISTACHIO_API __declspec(dllimport)
	#endif
#else
	#error Pistachio only supports Windows!
#endif
