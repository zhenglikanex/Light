#pragma once

#include "engine/renderer/camera.h"
#include "engine/rhi/device.h"
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

		explicit Renderer(rhi::Device* device,rhi::SwapChain* swap_cahin);

		// 设置当前帧统一变量,如相机，光源，环境参数
		void BeginScene(const OrthographicCamera& camera);

		// 提交渲染命令
		void Submit(rhi::GraphicsPipeline* pso, rhi::Buffer* vertex_buffer, rhi::Buffer* index_buffer, const glm::mat4& model_matrix, const glm::vec4& color);

		void EndScene();
	private:
		rhi::Device* device_;
		rhi::SwapChain* swap_cahin_;
		rhi::CommandListHandle command_list_;

		SceneData scene_data_;
	};
}