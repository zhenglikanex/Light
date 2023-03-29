#pragma once

#include <memory>

#include "engine/rhi/command_list.h"
#include "engine/renderer/camera.h"

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
			kMaterial
		};

		struct SceneData
		{
			glm::mat4 projection_matrix;
			glm::mat4 view_matrix;
			glm::mat4 view_projection_matrix;
		};

		// 设置当前帧统一变量,如相机，光源，环境参数
		static void BeginScene(rhi::CommandList* command_list,const OrthographicCamera& camera);
		
		// 提交渲染命令
		static void Submit(rhi::CommandList* command_list,rhi::GraphicsPipeline* pso, rhi::Buffer* vertex_buffer, rhi::Buffer* index_buffer, const glm::mat4& model_matrix, const glm::vec4& color);
		
		static void EndScene();
	private:
		static SceneData s_scene_data;
	};
}