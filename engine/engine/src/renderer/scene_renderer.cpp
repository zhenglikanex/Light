#include "engine/renderer/scene_renderer.h"
#include "engine/renderer/mesh.h"
#include "engine/renderer/editor_camera.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/render_pass.h"
#include "engine/renderer/shader_library.h"

#include "engine/asset/asset_manager.h"

#include "engine/core/application.h"

#include "engine/rhi/types.h"
#include "engine/rhi/buffer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace light
{
	SceneRenderer* SceneRenderer::s_instance = nullptr;

	void SceneRenderer::Init()
	{
		s_instance = new SceneRenderer();

		uint32_t width = Application::Get().GetWindow()->GetWidth();
		uint32_t height = Application::Get().GetWindow()->GetHeight();

		SetViewportSize(width, height);

		// todo:引擎内部应该换个加载方式?
		s_instance->shadow_shader_ = AssetManager::LoadAsset<Shader>("shaders/shadow.shader");
		s_instance->skybox_shader_ = AssetManager::LoadAsset<Shader>("shaders/skybox.shader");
		s_instance->skybox_shader_->SetDepthFunc(rhi::ComparisonFunc::kLessEqual);

		s_instance->shadow_material_ = MakeRef<Material>(s_instance->shadow_shader_);
		s_instance->shadow_material_->SetCullMode(rhi::CullMode::kFront);

		s_instance->skybox_material_ = MakeRef<Material>(s_instance->skybox_shader_);
	}

	void SceneRenderer::Shutdown()
	{
		delete s_instance;
		s_instance = nullptr;
	}

	SceneRendererSetting& SceneRenderer::GetSetting()
	{
		static SceneRendererSetting setting;
		return setting;
	}

	void SceneRenderer::CreateShadowPass()
	{
		rhi::Device* device = Application::Get().GetDevice();
		const SceneRendererSetting& setting = GetSetting();

		rhi::ClearValue clear_value;
		clear_value.depth_stencil.depth = 1;
		clear_value.depth_stencil.stencil = 0;

		rhi::TextureDesc direction_shadow_tex_desc;
		direction_shadow_tex_desc.format = rhi::Format::D32;
		direction_shadow_tex_desc.array_size = setting.shadow_setting.num_direction_light;
		direction_shadow_tex_desc.width = setting.shadow_setting.width;
		direction_shadow_tex_desc.height = setting.shadow_setting.height;
		direction_shadow_tex_desc.debug_name = "direction_shaow_map";

		rhi::TextureHandle direction_shadow_tex = device->CreateTexture(direction_shadow_tex_desc, &clear_value);
		
		RenderPassResources resources;
		resources.render_target.AttachAttachment(rhi::AttachmentPoint::kDepthStencil, direction_shadow_tex);

		s_instance->shadow_pass_ = MakeRef<RenderPass>(resources);
	}

	void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
	{
		s_instance->viewport_width_ = width;
		s_instance->viewport_height_ = height;

		CreateRenderPass();
	}

	void SceneRenderer::BeginScene(rhi::CommandList* command_list, const rhi::RenderTarget& render_target, EditorCamera& editor_camera, const Scene* scene)
	{
		s_instance->active_scene_ = scene;
		s_instance->final_render_target_ = render_target;

		RenderPassResources resources;
		resources.render_target = s_instance->final_render_target_;

		s_instance->final_pass_ = MakeRef<RenderPass>(resources);
		s_instance->camera_ = editor_camera;
	}

	void SceneRenderer::EndScene(rhi::CommandList* command_list)
	{
		Draw(command_list);

		s_instance->active_scene_ = nullptr;
		s_instance->draw_items_.clear();
		s_instance->lights_.clear();
	}

	void SceneRenderer::SubmitLight(const glm::vec3& direction, const glm::vec3& color, const glm::mat4& transform)
	{
		Light light;
		light.direction = glm::normalize(direction);
		light.color = color;
		light.transform = transform;

		s_instance->lights_.emplace_back(light);
	}

	void SceneRenderer::SubmitMesh(const Mesh* mesh, const std::vector<Ref<Material>>& materials, const glm::mat4& transform)
	{
		for (const auto& sub_mesh : *mesh)
		{
			Material* material = materials[sub_mesh.material_index];
			if (!material)
			{
				material = mesh->GetMaterial(sub_mesh.material_index);
			}

			if (material)
			{
				DrawItem& render_item = s_instance->draw_items_.emplace_back();

				render_item.vertex_buffer = mesh->GetVertexBuffer();
				render_item.index_buffer = mesh->GetIndexBuffer();
				render_item.base_vertex = sub_mesh.base_vertex;
				render_item.base_index = sub_mesh.base_index;
				render_item.index_count = sub_mesh.index_count;
				render_item.material = material;
				render_item.model_matrix = transform;
			}
		}
	}

	void SceneRenderer::Draw(rhi::CommandList* command_list)
	{
		if (GetSetting().equirectangular_map && s_instance->equirectangular_map_ != GetSetting().equirectangular_map)
		{
			s_instance->equirectangular_map_ = GetSetting().equirectangular_map;
			s_instance->environment_map_ = Renderer::CreateEnvironmentMap(command_list, s_instance->equirectangular_map_);
		}

		ShadowPass(command_list);
		GeometryPass(command_list);
		FinalPass(command_list);
	}

	void SceneRenderer::CreateRenderPass()
	{
		rhi::Device* device = Application::Get().GetDevice();

		rhi::ClearValue color_clear_value;
		color_clear_value.color[0] = 0;
		color_clear_value.color[1] = 0;
		color_clear_value.color[2] = 0;
		color_clear_value.color[3] = 1;

		rhi::ClearValue depth_clear_value;
		depth_clear_value.depth_stencil.depth = 1;
		depth_clear_value.depth_stencil.stencil = 0;

		rhi::TextureDesc color_tex_desc;
		color_tex_desc.width = s_instance->viewport_width_;
		color_tex_desc.height = s_instance->viewport_height_;
		color_tex_desc.format = rhi::Format::RGBA8_UNORM;
		color_tex_desc.is_render_target = true;

		rhi::TextureDesc depth_tex_desc;
		depth_tex_desc.width = s_instance->viewport_width_;
		depth_tex_desc.height = s_instance->viewport_height_;
		depth_tex_desc.format = rhi::Format::D24S8;

		{
			rhi::TextureHandle color_tex = device->CreateTexture(color_tex_desc, &color_clear_value);
			rhi::TextureHandle depth_tex = device->CreateTexture(depth_tex_desc, &depth_clear_value);

			rhi::RenderTarget geo_render_target;
			geo_render_target.AttachAttachment(rhi::AttachmentPoint::kColor0, color_tex);
			geo_render_target.AttachAttachment(rhi::AttachmentPoint::kDepthStencil, depth_tex);

			RenderPassResources resources;
			resources.render_target = geo_render_target;
			s_instance->geometry_pass_ = MakeRef<RenderPass>(resources);
		}

		CreateShadowPass();
	}

	void SceneRenderer::ShadowPass(rhi::CommandList* command_list)
	{
		if (s_instance->lights_.empty() || s_instance->draw_items_.empty())
		{
			return;
		}

		Renderer::BeginRenderPass(command_list, s_instance->shadow_pass_);

		const Light& light = s_instance->lights_.back();

		float aspect = GetSetting().shadow_setting.width / GetSetting().shadow_setting.height;

		glm::mat4 projection = glm::orthoLH_ZO(-50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f);
		Camera camera(projection);

		Renderer::BeginScene(command_list, camera, light.transform);

		for (const auto& draw_item : s_instance->draw_items_)
		{
			Renderer::Draw(
				command_list,
				s_instance->shadow_material_,
				draw_item.vertex_buffer,
				draw_item.index_buffer,
				draw_item.model_matrix,
				draw_item.index_count,
				draw_item.base_vertex,
				draw_item.base_index);
		}

		Renderer::EndScene(command_list);

		Renderer::EndRenderPass(command_list, s_instance->shadow_pass_);
	}

	void SceneRenderer::PreDpehtPass(rhi::CommandList* command_list)
	{

	}

	void SceneRenderer::GeometryPass(rhi::CommandList* command_list)
	{
		Renderer::BeginRenderPass(command_list,s_instance->geometry_pass_);

		Renderer::BeginScene(command_list, s_instance->camera_);

		int num_light = std::min(s_instance->lights_.size(), (size_t)Renderer::kMaxLight);

		for (int i = 0; i < num_light; ++i)
		{
			const Light& scene_light = s_instance->lights_[i];

			Renderer::Light light;
			light.color = scene_light.color;
			light.direction = scene_light.direction;

			float aspect = GetSetting().shadow_setting.width / GetSetting().shadow_setting.height;
			glm::mat4 projection = glm::orthoLH_ZO(-50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f);
			light.view_projection_matrix = projection * glm::inverse(scene_light.transform);
			Renderer::SetupLight(light);
		}

		for (const auto& draw_item : s_instance->draw_items_)
		{
			rhi::Texture* shadow_map = s_instance->shadow_pass_->GetResources().render_target.GetAttachment(rhi::AttachmentPoint::kDepthStencil).texture;
			draw_item.material->Set("gShadowMap", shadow_map);

			Renderer::Draw(
				command_list,
				draw_item.material,
				draw_item.vertex_buffer,
				draw_item.index_buffer,
				draw_item.model_matrix,
				draw_item.index_count,
				draw_item.base_vertex,
				draw_item.base_index);
		}

		if (s_instance->environment_map_)
		{
			s_instance->skybox_material_->Set("gEnvironmentMap", s_instance->environment_map_);
			Renderer::DrawSkybox(command_list, s_instance->skybox_material_);
		}

		Renderer::EndScene(command_list);

		Renderer::EndRenderPass(command_list, s_instance->geometry_pass_);
	}

	void SceneRenderer::FinalPass(rhi::CommandList* command_list)
	{
		Renderer::BeginRenderPass(command_list, s_instance->s_instance->final_pass_);

	 	Shader* hdr_shader = AssetManager::LoadAsset<Shader>("shaders/hdr.shader");
		
		hdr_shader->Set("gSourceMap", s_instance->geometry_pass_->GetResources().render_target.GetAttachment(rhi::AttachmentPoint::kColor0).texture);
		Renderer::DrawQuad(command_list, hdr_shader);

		Renderer::EndRenderPass(command_list, s_instance->s_instance->final_pass_);
	}
}
