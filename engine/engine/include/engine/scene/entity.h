#pragma once

#include "engine/core/core.h"
#include "engine/scene/scene.h"

#include "entt/entt.hpp"

namespace light
{
	
	class Entity
	{
	public:
		Entity(entt::entity handle, Scene* scene);
		Entity() = default;

		template<class T,class ... Args>
		void AddComponent(Args&& ... args)
		{
			LIGHT_ASSERT(!HasComponent<T>(), "Entity already has component!");
			scene_->registry_.emplace<T>(entity_handle_, std::forward<Args>(args)...);
		}

		template<class ... Ts>
		bool HasComponent() const
		{
			return scene_->registry_.all_of<Ts...>(entity_handle_);
		}

		template<class T>
		T& GetComponent() const
		{
			LIGHT_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return scene_->registry_.get<T>(entity_handle_);
		}

		template<class T>
		void RemoveComponent()
		{
			LIGHT_ASSERT(HasComponent<T>(), "Entity does not have component!");
			scene_->registry_.remove<T>(entity_handle_);
		}

		bool IsVaild() const;

		operator bool() const { return IsVaild(); }
	private:
		entt::entity entity_handle_ = entt::null;
		Scene* scene_ = nullptr;
	};
}