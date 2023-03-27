#include "engine/renderer/renderer.h"
#include "engine/renderer/render_command.h"

namespace light
{
	Renderer::Renderer(rhi::Device* device, rhi::SwapChain* swap_chain)
		: device_(device)
		, swap_cahin_(swap_chain)
		, command_list_(nullptr)
	{

	}

	void Renderer::BeginScene(const OrthographicCamera& camera)
	{
		command_list_ = device_->GetCommandList(rhi::CommandListType::kDirect);
		auto render_target = swap_cahin_->GetRenderTarget();
		command_list_->SetRenderTarget(render_target);
		command_list_->SetViewport(render_target.GetViewport());
		
		command_list_->SetScissorRect({ 0,0,std::numeric_limits<int32_t>::max(),std::numeric_limits<int32_t>::max() });

		constexpr float clear_color[] = { 1.0, 0.0, 0.0, 1.0 };
		command_list_->ClearTexture(render_target.GetAttachment(rhi::AttachmentPoint::kColor0).texture, clear_color);

		// todo:
		//设置当前帧统一变量, 如相机，光源，环境参数

		scene_data_.projection_matrix = camera.GetProjectionMatrix();
		scene_data_.view_matrix = camera.GetViewMatrix();
		scene_data_.view_projection_matrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::Submit(rhi::GraphicsPipeline* pso, rhi::Buffer* vertex_buffer, rhi::Buffer* index_buffer,const glm::mat4& model_matrix,const glm::vec4& color)
	{
		command_list_->SetGraphicsPipeline(pso);

		command_list_->SetGraphicsDynamicConstantBuffer(static_cast<uint32_t>(ParameterIndex::kSceneData), scene_data_);
		command_list_->SetGraphics32BitConstants(static_cast<uint32_t>(ParameterIndex::kModelMatrix), model_matrix);
		command_list_->SetGraphics32BitConstants(static_cast<uint32_t>(ParameterIndex::kMaterial), color);
		command_list_->SetVertexBuffer(0,vertex_buffer);
		command_list_->SetIndexBuffer(index_buffer);
		command_list_->SetPrimitiveTopology(rhi::PrimitiveTopology::kTriangleList);
		command_list_->DrawIndexed(index_buffer->GetDesc().size_in_bytes / index_buffer->GetDesc().stride, 1, 0, 0, 0);
	}

	void Renderer::EndScene()
	{
		command_list_->ExecuteCommandList();
		command_list_ = nullptr;
	}
}
