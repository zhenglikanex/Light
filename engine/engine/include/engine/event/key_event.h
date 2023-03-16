#pragma once

#include "event.h"

namespace light
{
	struct KeyPressedEvent : public Event
	{
		int keycode;
		int repeat_count = 0;

		KeyPressedEvent(int keycode, int repeat_count);

		std::string ToString() const override;

		EVENT_IMPL(KeyPressedEvent, EventType::kKeyPressed, EventCategory::kKey)
	};

	struct KeyReleasedEvent : public Event
	{
		int keycode;

		explicit KeyReleasedEvent(int keycode);

		std::string ToString() const override;

		EVENT_IMPL(KeyReleasedEvent, EventType::kKeyReleased, EventCategory::kKey)
	};
}