#include "log/log.h"

namespace light::log
{
	std::shared_ptr<spdlog::logger> g_engine_logger = nullptr;
	std::shared_ptr<spdlog::logger> g_game_logger = nullptr;

	void Init()
	{
		g_engine_logger = spdlog::stdout_color_mt("Engine");
		g_engine_logger->set_level(spdlog::level::trace);
		g_engine_logger->set_pattern("[%T.%F] [%n] [%^%l%$] %v");
		g_game_logger = spdlog::stdout_color_mt("Game");
		g_game_logger->set_level(spdlog::level::trace);
		g_game_logger->set_pattern("[%T.%F] [%n]   [%^%l%$] %v");
	}
}