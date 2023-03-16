#include "engine/event/mouse_event.h"
#include "spdlog/fmt/fmt.h"

namespace light
{
	MouseMovedEvent::MouseMovedEvent(float x, float y)
		: x(x)
		, y(y)
	{
	}

	std::string MouseMovedEvent::ToString() const
	{
		return fmt::format("{} : x = {} y = {}", GetName(), x, y);
	}

	MouseButtonPressedEvent::MouseButtonPressedEvent(int button, int repeat_count)
		: button(button)
		, repeat_count(repeat_count)
	{

	}

	std::string MouseButtonPressedEvent::ToString() const
	{
		return fmt::format("{} : button = {} repeat_count = {}", GetName(), button, repeat_count);
	}

	MouseButtonReleasedEvent::MouseButtonReleasedEvent(int button)
		: button(button)
	{

	}

	std::string MouseButtonReleasedEvent::ToString() const
	{
		return fmt::format("{} : button = {}", GetName(), button);
	}

	MouseScrolledEvent::MouseScrolledEvent(float x,float y)
		: x(x)
		, y(y)
	{
	}

	std::string MouseScrolledEvent::ToString() const
	{
		return fmt::format("{} : x = {} y = {}", GetName(), x, y);
	}
}