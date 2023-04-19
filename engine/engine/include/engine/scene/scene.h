#pragma once

#include "light_pch.h"

#include "engine/core/core.h"
#include "engine/core/timestep.h"
#include "engine/rhi/command_list.h"

#include "entt/entt.hpp"

namespace light
{
	class Entity;
	class Scene : public RefCounter
	{
	public:
		void OnUpdate(Timestep ts,rhi::CommandList* command_list);

		Entity CreateEntity(std::string_view name = "Entity");

		void DestroyEntity(Entity entity);

		void SetViewportSize(uint32_t width, uint32_t height);

		entt::registry& GetRegistry() { return registry_; }
	private:
		friend class Entity;

		entt::registry registry_;
	};
}
