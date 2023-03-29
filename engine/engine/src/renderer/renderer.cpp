#include "engine/renderer/renderer.h"
#include "engine/renderer/render_command.h"

namespace light
{
	Renderer::SceneData Renderer::s_scene_data = {};

	void Renderer::BeginScene(rhi::CommandList* command_list,const OrthographicCamera& camera)
	{
		// todo:
		//设置当前帧统一变量, 如相机，光源，环境参数
		s_scene_data.projection_matrix = camera.GetProjectionMatrix();
		s_scene_data.view_matrix = camera.GetViewMatrix();
		s_scene_data.view_projection_matrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::Submit(rhi::CommandList* command_list,rhi::GraphicsPipeline* pso, rhi::Buffer* vertex_buffer, rhi::Buffer* index_buffer,const glm::mat4& model_matrix,const glm::vec4& color)
	{
		command_list->SetGraphicsPipeline(pso);

		command_list->SetGraphicsDynamicConstantBuffer(static_cast<uint32_t>(ParameterIndex::kSceneData),s_scene_data);
		command_list->SetGraphics32BitConstants(static_cast<uint32_t>(ParameterIndex::kModelMatrix), model_matrix);
		command_list->SetGraphics32BitConstants(static_cast<uint32_t>(ParameterIndex::kMaterial), color);
		command_list->SetVertexBuffer(0,vertex_buffer);
		command_list->SetIndexBuffer(index_buffer);
		command_list->SetPrimitiveTopology(rhi::PrimitiveTopology::kTriangleList);
		command_list->DrawIndexed(index_buffer->GetDesc().size_in_bytes / index_buffer->GetDesc().stride, 1, 0, 0, 0);
	}

	void Renderer::EndScene()
	{
		
	}
}
