#pragma once

#include "engine/event/event.h"

namespace light
{
	struct WindowClosedEvent : public Event
	{
		std::string ToString() const override;

		EVENT_IMPL(WindowClosedEvent,EventType::kWindowsClosed,EventCategory::kWindows)
	};

	struct WindowResizedEvent : public Event
	{
		uint32_t width;
		uint32_t height;

		WindowResizedEvent(uint32_t width, uint32_t height);

		std::string ToString() const override;

		EVENT_IMPL(WindowResizedEvent, EventType::kWindowsResized, EventCategory::kWindows)
	};
}