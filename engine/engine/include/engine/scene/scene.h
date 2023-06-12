#pragma once

#include "light_pch.h"

#include "engine/core/base.h"
#include "engine/core/timestep.h"

#include "engine/rhi/command_list.h"

#include "entt/entt.hpp"

namespace light
{
	class Entity;
	class CameraComponent;
	class EditorCamera;

	class Scene : public RefCounter
	{
	public:
		void OnUpdateEditor(Timestep ts, rhi::CommandList* command_list,const rhi::RenderTarget& render_target,EditorCamera& editor_camera);

		void OnUpdateRuntime(Timestep ts, rhi::CommandList* command_list,const rhi::RenderTarget& render_target);

		Entity CreateEntity(std::string_view name = "Entity");

		Entity CreateEmptyEntity();

		void DestroyEntity(Entity entity);

		void SetViewportSize(uint32_t width, uint32_t height);

		void Each(const std::function<void(Entity)>& func);

		// todo：暂时的
		Entity GetPrimaryCameraEntity();

		entt::registry& GetRegistry() { return registry_; }
	private:
		friend class Entity;

		template<typename T>
		void OnComponentAdd(Entity e, T& component)
		{

		}

		template<>
		inline void OnComponentAdd<CameraComponent>(Entity e, CameraComponent& component);

		entt::registry registry_;
		uint32_t viewport_width_;
		uint32_t viewport_height_;
	};
}
