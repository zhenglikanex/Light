#pragma once

#include "event.h"

namespace light
{
	class KeyPressedEvent : public Event
	{
	public:
		explicit KeyPressedEvent(int keycode);

		std::string ToString() const;

		EVENT_IMPL(KeyPressedEvent, EventType::kKeyPressed, EventCategory::kKey)
	private:
		int keycode_;
		int repeat_count_;
	};
}