#include "engine/scene/scene.h"
#include "engine/scene/entity.h"
#include "engine/scene/components.h"
#include "engine/renderer/renderer2d.h"

namespace light
{

	void Scene::OnUpdate(Timestep ts, rhi::CommandList* command_list)
	{
		auto view = registry_.view<TransformComponent, SpriteRendererComponent>();
		for (entt::entity e : view)
		{
			const auto& [transform,sprite] = view.get<TransformComponent, SpriteRendererComponent>(e);
			Renderer2D::DrawQuad(command_list, transform, sprite.color);
		}
	}

	Entity Scene::CreateEntity(std::string_view name)
	{
		Entity entity(registry_.create(),this);

		entity.AddComponent<TagComponent>(name);
		entity.AddComponent<TransformComponent>();
		
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
	}
}
