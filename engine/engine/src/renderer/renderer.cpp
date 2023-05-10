#include "engine/renderer/renderer.h"
#include "engine/renderer/render_command.h"
#include "engine/renderer/editor_camera.h"
#include "engine/renderer/mesh.h"
#include "engine/core/application.h"

namespace light
{
	Renderer::SceneData Renderer::s_scene_data = {};
	Renderer::RenderData* Renderer::s_render_data = nullptr;

	void Renderer::Init()
	{
		s_render_data = new RenderData();
	}

	void Renderer::BeginScene(rhi::CommandList* command_list, const rhi::RenderTarget& render_target, const Camera& camera, const glm::mat4& transform)
	{
		s_scene_data.projection_matrix = camera.GetProjection();
		s_scene_data.view_matrix = glm::inverse(transform);
		s_scene_data.view_projection_matrix = s_scene_data.projection_matrix * s_scene_data.view_matrix;

		s_render_data->render_target = render_target;
	}

	void Renderer::BeginScene(rhi::CommandList* command_list, const rhi::RenderTarget& render_target, const EditorCamera& camera)
	{
		s_scene_data.projection_matrix = camera.GetProjectionMatrx();
		s_scene_data.view_matrix = camera.GetViewMatrix();
		s_scene_data.view_projection_matrix = camera.GetViewProjectionMatrix();

		s_render_data->render_target = render_target;
	}

	void Renderer::EndScene(rhi::CommandList* command_list)
	{

	}

	void Renderer::DrawMesh(rhi::CommandList* command_list, Material* material, rhi::Buffer* vertex_buffer, rhi::Buffer* index_buffer, const glm::mat4& model_matrix)
	{
		command_list->SetGraphicsPipeline(GetGraphicsPipeline(material,s_render_data->render_target));

		command_list->SetGraphicsDynamicConstantBuffer(static_cast<uint32_t>(ParameterIndex::kSceneData), s_scene_data);
		command_list->SetGraphics32BitConstants(static_cast<uint32_t>(ParameterIndex::kModelMatrix), model_matrix);

		command_list->SetVertexBuffer(0, vertex_buffer);
		command_list->SetIndexBuffer(index_buffer);

		command_list->SetPrimitiveTopology(rhi::PrimitiveTopology::kTriangleList);
		command_list->DrawIndexed(index_buffer->GetDesc().size_in_bytes / index_buffer->GetDesc().stride, 1, 0, 0, 0);
	}
	rhi::GraphicsPipeline* Renderer::GetGraphicsPipeline(Material* material, const rhi::RenderTarget& render_target)
	{
		size_t hash = 0;

		//todo hash
		
		auto it = s_render_data->pso_cache.find(hash);
		if (it != s_render_data->pso_cache.end())
		{
			return it->second;
		}

		auto result = s_render_data->pso_cache.emplace(hash, CreateGraphicsPipeline(material, render_target));
		return result.first->second;
	}

	rhi::GraphicsPipelineHandle Renderer::CreateGraphicsPipeline(Material* material, const rhi::RenderTarget& render_target)
	{
		rhi::Device* device = Application::Get().GetDevice();

		std::vector<rhi::VertexAttributeDesc> vertex_attributes =
		{
			{ "POSITION",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,position),false},
			{ "NORMAL",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,normal),false },
			{ "TANGENT",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,tangent),false },
			{ "BITANGENT",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,bitangent),false },
			{ "TEXCOORD",0,rhi::Format::RG32_FLOAT,0,offsetof(Vertex,texcoord),false},
		};

		rhi::BindingParameter scene_data_param;
		scene_data_param.InitAsConstantBufferView(0);

		rhi::BindingParameter model_matrix_param;
		model_matrix_param.InitAsConstantBufferView(1);

		rhi::BindingLayout* binding_layout = new rhi::BindingLayout(2);
		binding_layout->Add(static_cast<uint32_t>(ParameterIndex::kSceneData), scene_data_param);
		binding_layout->Add(static_cast<uint32_t>(ParameterIndex::kModelMatrix), model_matrix_param);

		rhi::GraphicsPipelineDesc pso_desc;
		pso_desc.input_layout = device->CreateInputLayout(vertex_attributes);
		pso_desc.binding_layout = rhi::BindingLayoutHandle::Create(binding_layout);
		pso_desc.vs = material->GetShader()->GetVS();
		pso_desc.ps = material->GetShader()->GetPS();
		pso_desc.blend_state.render_target[0].blend_enable = true;
		pso_desc.blend_state.render_target[0].src_blend = rhi::BlendFactor::kSrcAlpha;
		pso_desc.blend_state.render_target[0].dest_blend = rhi::BlendFactor::kInvSrcAlpha;
		pso_desc.primitive_type = rhi::PrimitiveTopology::kTriangleList;

		return device->CreateGraphicsPipeline(pso_desc, render_target);
	}
}
