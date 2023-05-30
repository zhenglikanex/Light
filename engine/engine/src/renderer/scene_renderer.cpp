#include "engine/renderer/scene_renderer.h"
#include "engine/renderer/mesh.h"
#include "engine/renderer/editor_camera.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/render_pass.h"
#include "engine/renderer/shader_library.h"
#include "engine/core/application.h"

#include "engine/rhi/buffer.h"

namespace light
{
	SceneRenderer* SceneRenderer::s_instance = nullptr;

	void SceneRenderer::Init()
	{
		s_instance = new SceneRenderer();

		uint32_t width = Application::Get().GetWindow()->GetWidth();
		uint32_t height = Application::Get().GetWindow()->GetHeight();

		SetViewportSize(width, height);
	}

	void SceneRenderer::Shutdown()
	{
		delete s_instance;
		s_instance = nullptr;
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
		
		Renderer::BeginScene(command_list, editor_camera);
	}

	void SceneRenderer::EndScene(rhi::CommandList* command_list)
	{
		Draw(command_list);

		Renderer::EndScene(command_list);

		s_instance->active_scene_ = nullptr;
		s_instance->draw_items_.clear();
	}

	void SceneRenderer::SubmitLight(const glm::vec3& direction, const glm::vec3& color)
	{
		Renderer::Light light;
		light.color = color;
		light.direction = glm::normalize(direction);

		Renderer::SetupLight(light);
	}

	void SceneRenderer::SubmitMesh(Mesh* mesh,const glm::mat4& transform)
	{
		for (const auto& sub_mesh : *mesh)
		{
			if (mesh->GetMaterial(sub_mesh.material_index))
			{
				DrawItem& render_item = s_instance->draw_items_.emplace_back();

				render_item.vertex_buffer = mesh->GetVertexBuffer();
				render_item.index_buffer = mesh->GetIndexBuffer();
				render_item.base_vertex = sub_mesh.base_vertex;
				render_item.base_index = sub_mesh.base_index;
				render_item.index_count = sub_mesh.index_count;
				render_item.material = mesh->GetMaterial(sub_mesh.material_index);
				render_item.model_matrix = transform;
			}
		}
	}

	void SceneRenderer::Draw(rhi::CommandList* command_list)
	{
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
	}

	void SceneRenderer::ShadowPass(rhi::CommandList* command_list)
	{
		
	}

	void SceneRenderer::PreDpehtPass(rhi::CommandList* command_list)
	{

	}

	void SceneRenderer::GeometryPass(rhi::CommandList* command_list)
	{
		Renderer::BeginRenderPass(command_list,s_instance->geometry_pass_);

		for (const auto& draw_item : s_instance->draw_items_)
		{
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

		Renderer::EndRenderPass(command_list, s_instance->geometry_pass_);
	}

	void SceneRenderer::FinalPass(rhi::CommandList* command_list)
	{
		Renderer::BeginRenderPass(command_list, s_instance->s_instance->final_pass_);

	 	Shader* hdr_shader = ShaderLibrary::Get().Get("hdr");
		
		hdr_shader->Set("gSourceMap", s_instance->geometry_pass_->GetResources().render_target.GetAttachment(rhi::AttachmentPoint::kColor0).texture);
		Renderer::DrawQuad(command_list, hdr_shader);

		Renderer::EndRenderPass(command_list, s_instance->s_instance->final_pass_);
	}
}
