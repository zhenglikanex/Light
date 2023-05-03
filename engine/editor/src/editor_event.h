#pragma once

#include "engine/event/event.h"
#include "engine/scene/entity.h"

namespace light::editor
{
	struct SelectEntityEvent : public Event
	{
		explicit SelectEntityEvent(Entity e);

		Entity entity;

		std::string ToString() const override;

		EVENT_IMPL(SelectEntityEvent, EventType::kSelectEntity, EventCategory::kEditor)
	};
}