#pragma once

#include "Peakforge/Core/Base.h"

#include <spdlog/spdlog.h>

namespace PF
{
	namespace Log
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

	namespace Core
	{
		#define LOG_CORE_TRACE(...)		Log::Log::GetCoreLogger()->trace(__VA_ARGS__)
		#define LOG_CORE_DEBUG(...)		Log::Log::GetCoreLogger()->debug(__VA_ARGS__) 
		#define LOG_CORE_INFO(...)		Log::Log::GetCoreLogger()->info(__VA_ARGS__) 
		#define LOG_CORE_WARN(...)		Log::Log::GetCoreLogger()->warn(__VA_ARGS__) 
		#define LOG_CORE_ERROR(...)		Log::Log::GetCoreLogger()->error(__VA_ARGS__) 
		#define LOG_CORE_CRITICAL(...)	Log::Log::GetCoreLogger()->critical(__VA_ARGS__)
	}
}


#define LOG_TRACE(...)		PF::Log::Log::GetAppLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)		PF::Log::Log::GetAppLogger()->debug(__VA_ARGS__) 
#define LOG_INFO(...)		PF::Log::Log::GetAppLogger()->info(__VA_ARGS__) 
#define LOG_WARN(...)		PF::Log::Log::GetAppLogger()->warn(__VA_ARGS__) 
#define LOG_ERROR(...)		PF::Log::Log::GetAppLogger()->error(__VA_ARGS__) 
#define LOG_CRITICAL(...)	PF::Log::Log::GetAppLogger()->critical(__VA_ARGS__)

