#pragma once

#include <memory>

#include "engine/renderer/camera.h"
#include "engine/renderer/material.h"

#include "engine/rhi/command_list.h"
#include "engine/rhi/graphics_pipeline.h"

#include "glm/glm.hpp"

namespace light
{
	class Renderer
	{
	public:
		enum class ParameterIndex
		{
			kSceneData = 0,
			kModelMatrix,
			kMaterial,
			kTextures,
			kSampler,
		};

		struct Light
		{
			glm::vec3 direction;
			float padding1;
			glm::vec3 color;
			float padding2;
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
			std::unordered_map<size_t, rhi::GraphicsPipelineHandle> pso_cache;
			rhi::RenderTarget render_target;
		};

		constexpr static uint32_t kMaxTextures = 32;

		static void Init();

		// 设置当前帧统一变量,如相机，光源，环境参数
		static void BeginScene(rhi::CommandList* command_list, const rhi::RenderTarget& render_target, const Camera& camera, const glm::mat4& transform);
		static void BeginScene(rhi::CommandList* command_list, const rhi::RenderTarget& render_target, const EditorCamera& camera);
		static void EndScene(rhi::CommandList* command_list);
		
		static void SetupLight(Light light);

		// 提交渲染命令
		static void DrawMesh(rhi::CommandList* command_list, const Material* material, rhi::Buffer* vertex_buffer, rhi::Buffer* index_buffer, const glm::mat4& model_matrix);
	private:
		static rhi::GraphicsPipeline* GetGraphicsPipeline(const Shader* shader, const rhi::RenderTarget& render_target);

		static rhi::GraphicsPipelineHandle CreateGraphicsPipeline(const Shader* shader, const rhi::RenderTarget& render_target);

		static SceneData s_scene_data;
		static RenderData* s_render_data;
	};
}