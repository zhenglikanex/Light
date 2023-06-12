#pragma once

#include "engine/core/base.h"
#include "engine/scene/scene.h"

#include "entt/entt.hpp"

namespace light
{
	class Entity
	{
	public:
		static const Entity kNull;

		Entity(entt::entity handle, Scene* scene);
		Entity() = default;

		template<class T,class ... Args>
		decltype(auto) AddComponent(Args&& ... args)
		{
			LIGHT_ASSERT(!HasComponent<T>(), "Entity already has component!");
			
			if constexpr (std::is_empty_v<T>)
			{
				return scene_->registry_.emplace<T>(entity_handle_, std::forward<Args>(args)...);
			}
			else
			{
				T& component = scene_->registry_.emplace<T>(entity_handle_, std::forward<Args>(args)...);
				scene_->OnComponentAdd(*this, component);
				return component;
			} 
		}

		template<class ... Ts>
		bool HasComponent() const
		{
			return scene_->registry_.all_of<Ts...>(entity_handle_);
		}

		template<class T>
		decltype(auto) GetComponent() const
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

		bool IsValid() const;

		bool operator==(const Entity& other) const { return scene_ == other.scene_ && entity_handle_ == other.entity_handle_; }
		bool operator!=(const Entity& other) const { return !(*this == other); }

		operator bool() const { return IsValid(); }
		operator uint32_t () const { return (uint32_t)entity_handle_; }
		operator entt::entity() const { return entity_handle_; }
	private:
		entt::entity entity_handle_ = entt::null;
		Scene* scene_ = nullptr;
	};
}