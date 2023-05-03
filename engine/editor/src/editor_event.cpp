#include "editor_event.h"

#include <format>

namespace light::editor
{
	SelectEntityEvent::SelectEntityEvent(Entity e)
		: entity(e)
	{
		
	}

	std::string SelectEntityEvent::ToString() const
	{
		return std::format("{} : select entity", GetName());
	}
}