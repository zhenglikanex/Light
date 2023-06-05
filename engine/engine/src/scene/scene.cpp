#include "engine/scene/scene.h"
#include "engine/scene/entity.h"
#include "engine/scene/components.h"

#include "engine/renderer/camera.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/renderer2d.h"
#include "engine/renderer/scene_renderer.h"
#include "engine/renderer/editor_camera.h"

namespace light
{
	void Scene::OnUpdateEditor(Timestep ts, rhi::CommandList* command_list,const rhi::RenderTarget& render_target, EditorCamera& editor_camera)
	{
		SceneRenderer::BeginScene(command_list,render_target,editor_camera,this);

		// draw 3d
		{
			{
				auto group = registry_.group<TransformComponent>(entt::get<MeshComponent>);

				for (entt::entity e : group)
				{
					const auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(e);
					
					SceneRenderer::SubmitMesh(mesh.mesh, transform.GetTransform());
				};
			}

			{
				auto view = registry_.view<TransformComponent, LightComponent>();
				
				for (entt::entity e : view)
				{
					const auto& [transform, light] = view.get<TransformComponent, LightComponent>(e);
					
					glm::vec3 direction = glm::quat(glm::radians(transform.rotation)) * glm::vec3(0, 0, 1);

					SceneRenderer::SubmitLight(direction, light.color,transform.GetTransform());
				}
			}

			SceneRenderer::EndScene(command_list);			
		}

		// draw 2d
		{
			Renderer2D::BeginScene(command_list, render_target, editor_camera);

			auto view = registry_.view<TransformComponent, SpriteRendererComponent>();
			for (entt::entity e : view)
			{
				const auto& [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(e);
				Renderer2D::DrawQuad(command_list, transform.GetTransform(), sprite.color);
			}

			Renderer2D::EndScene(command_list);
		}
	}

	void Scene::OnUpdateRuntime(Timestep ts, rhi::CommandList* command_list, const rhi::RenderTarget& render_target)
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
			auto view = registry_.view<TransformComponent, CameraComponent>();
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
				Renderer2D::BeginScene(command_list, render_target, *main_camera, camera_transform);

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
		viewport_height_ = width;
		viewport_height_ = height;

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

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = registry_.view<CameraComponent>();
		for (entt::entity e : view)
		{
			// find main camera
			auto& camera = view.get<CameraComponent>(e);
			if (camera.primary)
			{
				return Entity{ e,this };
			}
		}

		return Entity::kNull;
	}

	template<>
	inline void Scene::OnComponentAdd(Entity e, CameraComponent& component)
	{
		component.camera.SetViewportSize(viewport_width_, viewport_height_);
	}
}
