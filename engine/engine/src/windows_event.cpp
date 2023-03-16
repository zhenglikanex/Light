#include "engine/event/windows_event.h"
#include "spdlog/fmt/fmt.h"

namespace light
{
	std::string WindowClosedEvent::ToString() const
	{
		return GetName();
	}

	WindowResizedEvent::WindowResizedEvent(uint32_t width, uint32_t height)
		: width(width)
		, height(height)
	{

	}

	std::string WindowResizedEvent::ToString() const
	{
		return fmt::format("{}: width = {} height = {}", GetName(), width, height);
	}
}