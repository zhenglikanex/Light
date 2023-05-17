#include "engine/renderer/scene_renderer.h"
#include "engine/renderer/mesh.h"
#include "engine/renderer/renderer.h"

#include "engine/rhi/buffer.h"

namespace light
{
	SceneRenderer* SceneRenderer::s_instance = nullptr;

	void SceneRenderer::Init()
	{
		s_instance = new SceneRenderer();
	}

	void SceneRenderer::Shutdown()
	{
		delete s_instance;
		s_instance = nullptr;
	}

	void SceneRenderer::BeginScene(const Scene* scene)
	{
		s_instance->active_scene_ = scene;
	}

	void SceneRenderer::EndScene()
	{
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
	}
}
