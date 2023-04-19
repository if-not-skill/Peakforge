#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace PF
{
	class Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetAppLogger() { return s_AppLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_AppLogger;
	};
}

#define LOG_CORE_TRACE(...)		PF::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_DEBUG(...)		PF::Log::GetCoreLogger()->debug(__VA_ARGS__) 
#define LOG_CORE_INFO(...)		PF::Log::GetCoreLogger()->info(__VA_ARGS__) 
#define LOG_CORE_WARN(...)		PF::Log::GetCoreLogger()->warn(__VA_ARGS__) 
#define LOG_CORE_ERROR(...)		PF::Log::GetCoreLogger()->error(__VA_ARGS__) 
#define LOG_CORE_CRITICAL(...)	PF::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define LOG_TRACE(...)		PF::Log::GetAppLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)		PF::Log::GetAppLogger()->debug(__VA_ARGS__) 
#define LOG_INFO(...)		PF::Log::GetAppLogger()->info(__VA_ARGS__) 
#define LOG_WARN(...)		PF::Log::GetAppLogger()->warn(__VA_ARGS__) 
#define LOG_ERROR(...)		PF::Log::GetAppLogger()->error(__VA_ARGS__) 
#define LOG_CRITICAL(...)	PF::Log::GetAppLogger()->critical(__VA_ARGS__)

