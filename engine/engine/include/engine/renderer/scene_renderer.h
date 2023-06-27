#pragma once

#include "light_pch.h"

#include "engine/reflection/meta.h"
#include "engine/rhi/command_list.h"
#include "engine/renderer/editor_camera.h"
#include "engine/renderer/material.h"
#include "engine/renderer/shader.h"

#include "glm/glm.hpp"

namespace light
{
	class Mesh;
	class Scene;
	class Shader;
	class Material;
	class RenderPass;
	class EditorCamera;

	struct META() SceneRendererSetting
	{
		struct META() ShadowSetting
		{
			uint32_t num_direction_light= 1;
			uint32_t width = 8192;
			uint32_t height = 8192;
		};

		ShadowSetting shadow_setting;
	};

	// 解耦场景与渲染相关的代码,将渲染相关的代码放到这里
	class SceneRenderer
	{
	public:
		static void Init();
		static void Shutdown();

		static SceneRendererSetting& GetSetting();

		static void CreateShadowPass();

		static void SetViewportSize(uint32_t width, uint32_t height);

		static void BeginScene(rhi::CommandList* command_list, const rhi::RenderTarget& render_target, EditorCamera& editor_camera, const Scene* scene);
		static void EndScene(rhi::CommandList* command_list);
		
		static void SubmitLight(const glm::vec3& direction, const glm::vec3& color,const glm::mat4& transform);
		static void SubmitMesh(const Mesh* mesh,const std::vector<Ref<Material>>& materials,const glm::mat4& transform);

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

		struct Light
		{
			glm::vec3 position;
			glm::vec3 direction;
			glm::vec3 color;
			glm::mat4 transform;
		};

		static SceneRenderer* s_instance;

		const Scene* active_scene_;

		uint32_t viewport_width_;
		uint32_t viewport_height_;

		rhi::RenderTarget final_render_target_;

		Ref<Shader> shadow_shader_;
		Ref<Material> shadow_material_;

		Ref<RenderPass> shadow_pass_;
		Ref<RenderPass> geometry_pass_;
		Ref<RenderPass> final_pass_;

		EditorCamera camera_;

		// 收集所有场景渲染对象方便进行全局优化
		std::vector<DrawItem> draw_items_;
		std::vector<Light> lights_;
	};
}