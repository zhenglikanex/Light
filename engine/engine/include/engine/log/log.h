#pragma once

#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/stopwatch.h"

namespace light::log
{
	using Stopwatch = spdlog::stopwatch;

	extern std::shared_ptr<spdlog::logger> g_engine_logger;
	extern std::shared_ptr<spdlog::logger> g_game_logger;

	extern void Init();
}

#if DEBUG

#define	LOG_ENGINE_TRACE(...) light::log::g_engine_logger->trace(__VA_ARGS__);
#define LOG_ENGINE_DEBUG(...) light::log::g_engine_logger->debug(__VA_ARGS__);
#define	LOG_ENGINE_INFO(...) light::log::g_engine_logger->info(__VA_ARGS__);
#define	LOG_ENGINE_WARN(...) light::log::g_engine_logger->warn(__VA_ARGS__);
#define	LOG_ENGINE_ERROR(...) light::log::g_engine_logger->error(__VA_ARGS__);
#define	LOG_ENGINE_CRITICAL(...) light::log::g_engine_logger->critical(__VA_ARGS__);

#define	LOG_GAME_TRACE(...) light::log::g_game_logger->trace(__VA_ARGS__);
#define LOG_GAME_DEBUG(...) light::log::g_game_logger->debug(__VA_ARGS__);
#define	LOG_GAME_INFO(...) light::log::g_game_logger->info(__VA_ARGS__);
#define	LOG_GAME_WARN(...) light::log::g_game_logger->warn(__VA_ARGS__);
#define	LOG_GAME_ERROR(...) light::log::g_game_logger->error(__VA_ARGS__);
#define	LOG_GAME_CRITICAL(...) light::log::g_game_logger->critical(__VA_ARGS__);

#else

#define	LOG_ENGINE_TRACE(...)
#define LOG_ENGINE_DEBUG(...)
#define	LOG_ENGINE_INFO(...) 
#define	LOG_ENGINE_WARN(...)
#define	LOG_ENGINE_ERR(...)
#define	LOG_ENGINE_CRITICAL(...)

#define	LOG_GAME_TRACE(...)
#define LOG_GAME_DEBUG(...)
#define	LOG_GAME_INFO(...) 
#define	LOG_GAME_WARN(...)
#define	LOG_GAME_ERR(...)
#define	LOG_GAME_CRITICAL(...)
	
#endif