#pragma once

#include "light_pch.h"

#include "engine/rhi/command_list.h"

#include "glm/glm.hpp"

namespace light
{
	class Mesh;
	class Scene;
	class Material;
	class RenderPass;
	class EditorCamera;

	// 解耦场景与渲染相关的代码,将渲染相关的代码放到这里
	class SceneRenderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void SetViewportSize(uint32_t width, uint32_t height);

		static void BeginScene(rhi::CommandList* command_list, const rhi::RenderTarget& render_target, EditorCamera& editor_camera, const Scene* scene);
		static void EndScene(rhi::CommandList* command_list);
		
		static void SubmitLight(const glm::vec3& direction, const glm::vec3& color);
		static void SubmitMesh(Mesh* mesh,const glm::mat4& transform);

		static void Draw(rhi::CommandList* command_list);
	private:
		static void CreateRenderPass();

		static void ShadowPass(rhi::CommandList* command_list);
		static void PreDpehtPass(rhi::CommandList* command_list);
		static void GeometryPass(rhi::CommandList* command_list);
		static void FinalPass(rhi::CommandList* command_list);

		struct DrawItem
		{
			rhi::Buffer* vertex_buffer;
			rhi::Buffer* index_buffer;

			uint32_t base_vertex;
			uint32_t base_index;
			uint32_t index_count;

			Material* material;

			glm::mat4 model_matrix;
		};

		static SceneRenderer* s_instance;

		const Scene* active_scene_;

		uint32_t viewport_width_;
		uint32_t viewport_height_;

		rhi::RenderTarget final_render_target_;

		Ref<RenderPass> shadow_pass_;
		Ref<RenderPass> geometry_pass_;
		Ref<RenderPass> final_pass_;

		// 收集所有场景渲染对象方便进行全局优化
		std::vector<DrawItem> draw_items_;
	};
}