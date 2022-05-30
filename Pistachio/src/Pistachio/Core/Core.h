#pragma once

#include <memory>


#define BIT(x) (1 << x)

#define PST_BIND_EVENT_FUNCTION(function) [this](auto&&... args) -> decltype(auto) { return this->function(std::forward<decltype(args)>(args)...); }


#ifdef _WIN32
	#ifdef _WIN64
		#define PST_PLATFORM_WINDOWS
	#else
		#error "x86 Builds are not supported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	/* TARGET_OS_MAC exists on all platforms,
	 * so we must check all of them (in this order)
	 * to ensure that we're running on MacOS */
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define PST_PLATFORM_IOS
		#error "IOS is currently not supported!"
	#elif TARGET_OS_MAC == 1
		#define PST_PLATFORM_MACOS
		#error "MacOS is currently not supported!"
	#else
		#error "Unrecognised Apple platform!"
	#endif

// Must check __ANDROID__ before __linux__ since Android runs on linux
#elif defined(__ANDROID__)
	#define PST_PLATFORM_ANDROID
	#error "Android is currently not supported!"
#elif defined(__linux__)
	#define PST_PLATFORM_LINUX
	#error "Linux is currently not supported!"
#else
	#error "Unrecognised platform!"
#endif


#ifdef PST_DEBUG
	#if defined(PST_PLATFORM_WINDOWS)
		#define PST_DEBUGBREAK() __debugbreak()
	#elif defined(PST_PLATFORM_LINUX)
		#include <signals.h>
		#define PST_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak"
	#endif
	#define PST_ENABLE_ASSERTS
#else
	#define PST_DEBUGBREAK()
#endif


// TODO: Allow Assert macro to take no varadic arguments
#ifdef PST_ENABLE_ASSERTS
	#define PST_CORE_ASSERT(x, ...) { if(!(x)) { PST_CORE_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); PST_DEBUGBREAK(); } }
	#define PST_ASSERT(x, ...)      { if(!(x)) { PST_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); PST_DEBUGBREAK(); } }
#else
	#define PST_CORE_ASSERT(x, ...) 
	#define PST_ASSERT(x, ...)      
#endif


namespace Pistachio {

	template<typename T>
	using Scoped = std::unique_ptr<T>;

	template<typename T, typename... Args>
	constexpr Scoped<T> CreateScoped(Args&&... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&&... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
