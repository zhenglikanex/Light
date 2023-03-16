#pragma once

#include "engine/event/event.h"

namespace light
{
	struct MouseButtonPressedEvent : public Event
	{
		int button;
		int repeat_count = 0;

		MouseButtonPressedEvent(int button, int repeat_count);

		std::string ToString() const override;

		EVENT_IMPL(MouseButtonPressedEvent, EventType::kMouseButtonPressed, EventCategory::kMouse)
	};

	struct MouseButtonReleasedEvent : public Event
	{
		int button;

		explicit MouseButtonReleasedEvent(int button);

		std::string ToString() const override;

		EVENT_IMPL(MouseButtonReleasedEvent,EventType::kMouseButtonReleased,EventCategory::kMouse)
	};

	struct MouseMovedEvent : public Event
	{
		float x;
		float y;

		explicit MouseMovedEvent(float x, float y);

		std::string ToString() const override;

		EVENT_IMPL(MouseMovedEvent, EventType::kMouseMoved, EventCategory::kMouse)
	};

	struct MouseScrolledEvent : public Event
	{
		float x;
		float y;

		explicit MouseScrolledEvent(float x,float y);

		std::string ToString() const override;

		EVENT_IMPL(MouseScrolledEvent,EventType::kMouseScrolled,EventCategory::kMouse)
	};
}