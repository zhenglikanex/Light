#include "engine/scene/scene.h"
#include "engine/scene/entity.h"
#include "engine/scene/components.h"
#include "engine/renderer/camera.h"
#include "engine/renderer/renderer2d.h"

namespace light
{

	void Scene::OnUpdate(Timestep ts, rhi::CommandList* command_list)
	{
		// update scripts
		registry_.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
			if (!nsc.script_instance)
			{
				nsc.script_instance = nsc.script_instance_constructor();
				nsc.script_instance->entity = Entity(entity, this);
				nsc.script_instance->OnCreate();
			}
			nsc.script_instance->OnUpdate(ts);
		});

		Camera* main_camera = nullptr;
		glm::mat4 camera_transform;

		{
			auto view = registry_.view<TransformComponent,CameraComponent>();
			for (auto e : view)
			{
				// find main camera
				const auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(e);
				if (camera.primary)
				{
					main_camera = &camera.camera;
					camera_transform = transform.GetTransform();
					break;
				}
			}
		}

		{
			if (main_camera)
			{
				Renderer2D::BeginScene(command_list, *main_camera, camera_transform);

				auto group = registry_.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (entt::entity e : group)
				{
					const auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(e);
					Renderer2D::DrawQuad(command_list, transform.GetTransform(), sprite.color);
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

	Entity Scene::CreateEmptyEntity()
	{
		Entity entity(registry_.create(), this);
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		registry_.destroy(entity);
	}

	void Scene::SetViewportSize(uint32_t width, uint32_t height)
	{
		// set cameracomponent viewportsize
		auto view = registry_.view<CameraComponent>();
		for (auto e : view)
		{
			auto& camera = view.get<CameraComponent>(e);
			if (!camera.aspect_ratio_fixed)
			{
				camera.camera.SetViewportSize(width, height);
			}
		}
	}

	void Scene::Each(const std::function<void(Entity)>& func)
	{
		registry_.each([this,&func](entt::entity e) {
			func(Entity{ e,this });
			});
	}
}
