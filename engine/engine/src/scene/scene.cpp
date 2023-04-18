#include "engine/scene/scene.h"
#include "engine/scene/entity.h"
#include "engine/scene/components.h"
#include "engine/renderer/camera.h"
#include "engine/renderer/renderer2d.h"

namespace light
{

	void Scene::OnUpdate(Timestep ts, rhi::CommandList* command_list)
	{
		Camera* main_camera = nullptr;
		glm::mat4* camera_transform = nullptr;

		{
			auto view = registry_.view<TransformComponent,CameraComponent>();
			for (auto e : view)
			{
				// find main camera
				const auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(e);
				if (camera.primary)
				{
					main_camera = &camera.camera;
					camera_transform = &transform.transform;
					break;
				}
			}
		}

		{
			if (main_camera)
			{
				Renderer2D::BeginScene(command_list, *main_camera, *camera_transform);

				auto group = registry_.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (entt::entity e : group)
				{
					const auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(e);
					Renderer2D::DrawQuad(command_list, transform, sprite.color);
				}

				Renderer2D::EndScene(command_list);
			}
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
