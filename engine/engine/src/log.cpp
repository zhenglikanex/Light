#include "engine/log/log.h"

namespace light
{
	std::shared_ptr<spdlog::logger> Log::g_engine_logger;
	std::shared_ptr<spdlog::logger> Log::g_game_logger;

	void Log::Init()
	{
		// Init spdlog.
		spdlog::init_thread_pool(8192, 1);
		auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();

		std::vector<spdlog::sink_ptr> sinks{ stdout_sink, msvc_sink };

		g_engine_logger = std::make_shared<spdlog::async_logger>("Engine", sinks.begin(), sinks.end(),
			spdlog::thread_pool(), spdlog::async_overflow_policy::block);
		g_engine_logger->set_level(spdlog::level::trace);
		g_engine_logger->set_pattern("[%T] [%n] [%^%l%$] %v");

		g_game_logger = std::make_shared<spdlog::async_logger>("Game", sinks.begin(), sinks.end(),
			spdlog::thread_pool(), spdlog::async_overflow_policy::block);
		g_game_logger->set_level(spdlog::level::trace);
		g_game_logger->set_pattern("[%T] [%n]   [%^%l%$] %v");
	}
}