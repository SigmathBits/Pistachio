#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"


// Core log macros
#define PST_CORE_TRACE(...)     ::Pistachio::Log::CoreLogger()->trace(__VA_ARGS__)
#define PST_CORE_INFO(...)      ::Pistachio::Log::CoreLogger()->info(__VA_ARGS__)
#define PST_CORE_WARN(...)      ::Pistachio::Log::CoreLogger()->warn(__VA_ARGS__)
#define PST_CORE_ERROR(...)     ::Pistachio::Log::CoreLogger()->error(__VA_ARGS__)
#define PST_CORE_CRITICAL(...)  ::Pistachio::Log::CoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define PST_TRACE(...)          ::Pistachio::Log::ClientLogger()->trace(__VA_ARGS__)
#define PST_INFO(...)           ::Pistachio::Log::ClientLogger()->info(__VA_ARGS__)
#define PST_WARN(...)           ::Pistachio::Log::ClientLogger()->warn(__VA_ARGS__)
#define PST_ERROR(...)          ::Pistachio::Log::ClientLogger()->error(__VA_ARGS__)
#define PST_CRITICAL(...)       ::Pistachio::Log::ClientLogger()->critical(__VA_ARGS__)


namespace Pistachio {

	class Log {
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& CoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& ClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

