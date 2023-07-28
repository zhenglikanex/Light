#include "light_pch.h"
#include "engine/scene/entity.h"
#include "engine/scene/scene.h"
#include "engine/scene/components.h"

namespace light
{
	const Entity Entity::kNull;

	Entity::Entity(entt::entity handle, Scene* scene)
		: entity_handle_(handle)
		, scene_(scene)
	{
		
	}

	bool Entity::IsValid() const
	{
		return entity_handle_ != entt::null && scene_ != nullptr && scene_->registry_.valid(entity_handle_);
	}

	void Entity::SetParent(Entity parent) const
	{
		LIGHT_ASSERT(!HasParent(), "Entity Has Parent!");

		parent.AddChild(*this);
	}

	void Entity::AddChild(Entity child) const
	{
		LIGHT_ASSERT(!child.HasParent(), "Child Has Parent!");

		RelationshipComponent& parent_component = GetComponent<RelationshipComponent>();
		RelationshipComponent& child_component = child.GetComponent<RelationshipComponent>();

		if (parent_component.first != entt::null)
		{
			RelationshipComponent& first_component = scene_->registry_.get<RelationshipComponent>(parent_component.first);
			child_component.next = parent_component.first;
			first_component.prev = child;
			parent_component.first = child;
		}
		else
		{
			parent_component.first = child;
		}
		
		++parent_component.children;
		child_component.parent = entity_handle_;
	}

	void Entity::RemoveChild(Entity child) const
	{
		RelationshipComponent& component = GetComponent<RelationshipComponent>();
		entt::entity current = component.first;
		while (current != entt::null)
		{
			Entity e(current, scene_);

			if (current == child)
			{
				RelationshipComponent& current_component = e.GetComponent<RelationshipComponent>();
				if (current_component.prev != entt::null)
				{
					RelationshipComponent& prev_component = scene_->registry_.get<RelationshipComponent>(current_component.prev);
					prev_component.next = current_component.next;
				}
				else
				{
					component.first = current_component.next;
				}

				if (current_component.next != entt::null)
				{
					RelationshipComponent& next_component = scene_->registry_.get<RelationshipComponent>(current_component.next);
					next_component.prev = current_component.prev;
				}

				--component.children;
				return;
			}

			current = (e.GetComponent<RelationshipComponent>().next);
		}
	}

	bool Entity::HasParent() const
	{
		return GetComponent<RelationshipComponent>().parent != entt::null;
	}

	Entity Entity::GetChild(size_t index) const
	{
		RelationshipComponent& component = GetComponent<RelationshipComponent>();

		LIGHT_ASSERT(index < component.children, "!");
		
		entt::entity current = component.first;
		size_t i = 0;
		while (current != entt::null)
		{
			Entity e(current, scene_);

			if (i++ == index)
			{
				return e;
			}

			current = (e.GetComponent<RelationshipComponent>().next);
		}
	}               

	size_t Entity::GetNumChildren() const
	{
		return GetComponent<RelationshipComponent>().children;
	}

} // namespace light