#pragma once

#include "engine/renderer/camera.h"
#include "engine/rhi/buffer.h"
#include "engine/rhi/graphics_pipeline.h"
#include "engine/rhi/command_list.h"
#include "engine/rhi/sampler.h"
#include "glm/glm.hpp"

namespace light
{
	class Renderer2D
	{
	public:
		enum class ParameterIndex
		{
			kSceneData = 0,
			kModelMatrix,
			QuadMaterial,
			kTexture,
			kSampler
		};

		struct Storage
		{
			rhi::TextureHandle white_texture;
			rhi::BufferHandle vertex_buffer;
			rhi::BufferHandle index_buffer;
			rhi::GraphicsPipelineHandle texture_pso;
			rhi::SamplerHandle point_sampler;
		};

		struct SceneData
		{
			glm::mat4 projection_matrix;
			glm::mat4 view_matrix;
			glm::mat4 view_projection_matrix;
		};

		struct QuadMaterial
		{
			glm::vec4 color;
			float tiling_factor;
		};

		static void Init();

		static void Shutdown();

		// 设置当前帧统一变量,如相机，光源，环境参数
		static void BeginScene(rhi::CommandList* command_list, const OrthographicCamera& camera);

		static void EndScene();

		static void DrawQuad(rhi::CommandList* command_list, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(rhi::CommandList* command_list, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(rhi::CommandList* command_list, const glm::vec2& position, const glm::vec2& size, rhi::Texture* texture, float tiling_factor = 1.0f, glm::vec4 tint_color = glm::vec4(1.0f));
		static void DrawQuad(rhi::CommandList* command_list, const glm::vec3& position, const glm::vec2& size, rhi::Texture* texture, float tiling_factor = 1.0f, glm::vec4 tint_color = glm::vec4(1.0f));

		static void DrawRotationQuad(rhi::CommandList* command_list, const glm::vec2& position, float rotation, const glm::vec2& size, const glm::vec4& color);
		static void DrawRotationQuad(rhi::CommandList* command_list, const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& color);
		static void DrawRotationQuad(rhi::CommandList* command_list, const glm::vec2& position, float rotation, const glm::vec2& size, rhi::Texture* texture, float tiling_factor = 1.0f, glm::vec4 tint_color = glm::vec4(1.0f));
		static void DrawRotationQuad(rhi::CommandList* command_list, const glm::vec3& position, float rotation, const glm::vec2& size, rhi::Texture* texture, float tiling_factor = 1.0f, glm::vec4 tint_color = glm::vec4(1.0f));

	private:
		static Storage* s_storage;
		static SceneData s_scene_data;
	};
}