#include "engine/event/windows_event.h"

#include <format>

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
		return std::format("{}: width = {} height = {}", GetName(), width, height);
	}

	std::string WindowFileDropedEvent::ToString() const
	{
		return GetName();
	}
}