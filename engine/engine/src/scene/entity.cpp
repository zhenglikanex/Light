#include "light_pch.h"
#include "engine/scene/entity.h"
#include "engine/scene/scene.h"

namespace light
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: entity_handle_(handle)
		, scene_(scene)
	{
		
	}

	bool Entity::IsVaild() const
	{
		return entity_handle_ != entt::null && scene_ != nullptr && scene_->registry_.valid(entity_handle_);
	}

} // namespace light