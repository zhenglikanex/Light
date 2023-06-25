#pragma once

#include <memory>

#include "engine/renderer/camera.h"
#include "engine/renderer/material.h"
#include "engine/renderer/render_pass.h"
#include "engine/renderer/shader_library.h"

#include "engine/rhi/command_list.h"
#include "engine/rhi/graphics_pipeline.h"
#include "engine/rhi/sampler.h"

#include "glm/glm.hpp"

namespace light
{
	class Renderer
	{
	public:
		struct QuadVertex
		{
			glm::vec3 position;
			glm::vec2 texcoord;
		};

		struct Light
		{
			glm::vec3 direction;
			float padding1;
			glm::vec3 color;
			float padding2;
			glm::mat4 view_projection_matrix;
		};

		struct SceneData
		{
			glm::mat4 projection_matrix;
			glm::mat4 view_matrix;
			glm::mat4 view_projection_matrix;
			glm::vec3 camera_position;
			float padding1;
			Light light;
		};

		struct RenderData
		{
			rhi::BufferHandle quad_vertex_buffer;
			rhi::BufferHandle quad_index_buffer;
			std::unordered_map<size_t, rhi::GraphicsPipelineHandle> pso_cache;
			rhi::RenderTarget render_target;
			rhi::SamplerHandle sampler;
			//todo:
			std::unordered_map<std::string, rhi::SamplerHandle> samplers;
		};

		constexpr static uint32_t kMaxTextures = 32;

		static void Init();
		static void Shutdown();

		// 设置当前帧统一变量,如相机，光源，环境参数
		static void BeginScene(rhi::CommandList* command_list,const Camera& camera, const glm::mat4& transform);
		static void BeginScene(rhi::CommandList* command_list,const EditorCamera& camera);
		static void EndScene(rhi::CommandList* command_list);
		
		static void BeginRenderPass(rhi::CommandList* command_list, RenderPass* render_pass);
		static void EndRenderPass(rhi::CommandList* command_list, RenderPass* render_pass);

		static void SetupLight(Light light);
		
		static void SetupRenderTarget(rhi::CommandList* command_list, const rhi::RenderTarget& render_target);

		// 提交渲染命令
		static void Draw(
			rhi::CommandList* command_list,
			const Material* material,
			rhi::Buffer* vertex_buffer,
			rhi::Buffer* index_buffer,
			const glm::mat4& model_matrix,
			uint32_t index_count,
			uint32_t base_vertex = 0, 
			uint32_t base_index = 0);

		static void DrawQuad(rhi::CommandList* command_list, const Shader* shader, glm::vec2 position = glm::vec2(0), glm::vec2 scale = glm::vec2(1));
	private:
		static rhi::GraphicsPipeline* GetGraphicsPipeline(const Shader* shader, const rhi::RenderTarget& render_target,size_t vertex_type);

		static rhi::GraphicsPipelineHandle CreateGraphicsPipeline(const Shader* shader, const rhi::RenderTarget& render_target,size_t vertex_type);

		static SceneData s_scene_data;
		static RenderData* s_render_data;
	};
}