#include "engine/renderer/renderer2d.h"
#include "engine/core/application.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace light
{
	Renderer2D::Storage* Renderer2D::s_storage = nullptr;
	Renderer2D::SceneData Renderer2D::s_scene_data = {};

	void Renderer2D::Init()
	{
		s_storage = new Storage();

		std::vector<rhi::VertexAttributeDesc> vertex_attributes =
		{
			{"POSITION",0,rhi::Format::RGB32_FLOAT,0,0u,false}
		};

		std::vector<glm::vec3> vertices
		{
			{ -0.5f, -0.5f, 0.5f },
			{ -0.5f, +0.5f, 0.5f },
			{ +0.5f, +0.5f, 0.5f },
			{ +0.5f, -0.5f, 0.5f },
		};

		std::vector<uint16_t> indices
		{
			0,1,2,
			0,2,3,
		};

		auto device = Application::Get().GetDevice();
		auto command_list = device->GetCommandList(rhi::CommandListType::kCopy);

		rhi::BufferDesc vertex_desc;
		vertex_desc.type = rhi::BufferType::kVertex;
		vertex_desc.format = rhi::Format::RGB32_FLOAT;
		vertex_desc.size_in_bytes = vertices.size() * sizeof(glm::vec3);
		vertex_desc.stride = sizeof(glm::vec3);
		s_storage->vertex_buffer = device->CreateBuffer(vertex_desc);

		command_list->WriteBuffer(s_storage->vertex_buffer, reinterpret_cast<uint8_t*>(vertices.data()), vertex_desc.size_in_bytes);

		rhi::BufferDesc index_desc;
		index_desc.format = rhi::Format::R16_UINT;
		index_desc.type = rhi::BufferType::kIndex;
		index_desc.size_in_bytes = sizeof(uint16_t) * indices.size();
		index_desc.stride = sizeof(uint16_t);
		s_storage->index_buffer = device->CreateBuffer(index_desc);

		command_list->WriteBuffer(s_storage->index_buffer, reinterpret_cast<uint8_t*>(indices.data()), index_desc.size_in_bytes);

		command_list->ExecuteCommandList();

		rhi::BindingParameter scene_data_param;
		scene_data_param.InitAsConstantBufferView(0);

		rhi::BindingParameter model_matrix_param;
		model_matrix_param.InitAsConstants(sizeof(glm::mat4) / 4, 1);

		rhi::BindingParameter material_param;
		material_param.InitAsConstants(sizeof(glm::vec4) / 4, 2);

		rhi::BindingLayout* binding_layout = new rhi::BindingLayout(1);
		binding_layout->Add(static_cast<uint32_t>(ParameterIndex::kSceneData), scene_data_param);
		binding_layout->Add(static_cast<uint32_t>(ParameterIndex::kModelMatrix), model_matrix_param);
		binding_layout->Add(static_cast<uint32_t>(ParameterIndex::kMaterial), material_param);

		rhi::ShaderHandle vertex_shader = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kVertex, "assets/shaders/color.hlsl", "VS", "vs_5_0");
		rhi::ShaderHandle pixel_shader = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kPixel, "assets/shaders/color.hlsl", "PS", "ps_5_0");

		rhi::GraphicsPipelineDesc pso_desc;
		pso_desc.input_layout = device->CreateInputLayout(std::move(vertex_attributes));
		pso_desc.binding_layout = rhi::BindingLayoutHandle::Create(binding_layout);
		pso_desc.vs = vertex_shader;
		pso_desc.ps = pixel_shader;
		pso_desc.primitive_type = rhi::PrimitiveTopology::kTriangleList;

		rhi::RenderTarget render_target = Application::Get().GetSwapChain()->GetRenderTarget();
		s_storage->default_pso = device->CreateGraphicsPipeline(pso_desc, render_target);
	}

	void Renderer2D::Shutdown()
	{
		delete s_storage;
		s_storage = nullptr;
	}

	void Renderer2D::BeginScene(rhi::CommandList* command_list, const OrthographicCamera& camera)
	{
		s_scene_data.projection_matrix = camera.GetProjectionMatrix();
		s_scene_data.view_matrix = camera.GetViewMatrix();
		s_scene_data.view_projection_matrix = camera.GetViewProjectionMatrix();
	}

	void Renderer2D::EndScene()
	{

	}

	void Renderer2D::DrawQuad(rhi::CommandList* command_list, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(command_list, s_storage->default_pso, glm::vec3(position, 0.0f), size, color);
	}

	void Renderer2D::DrawQuad(rhi::CommandList* command_list, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(command_list, s_storage->default_pso, position, size, color);
	}

	void Renderer2D::DrawQuad(rhi::CommandList* command_list, rhi::GraphicsPipeline* pso, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(command_list, pso, glm::vec3(position, 0.0f), size, color);
	}

	void Renderer2D::DrawQuad(rhi::CommandList* command_list, rhi::GraphicsPipeline* pso, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), position);

		command_list->SetGraphicsPipeline(pso);
		command_list->SetGraphicsDynamicConstantBuffer(static_cast<uint32_t>(ParameterIndex::kSceneData), s_scene_data);
		command_list->SetGraphics32BitConstants(static_cast<uint32_t>(ParameterIndex::kModelMatrix), model_matrix);
		command_list->SetGraphics32BitConstants(static_cast<uint32_t>(ParameterIndex::kMaterial), color);
		command_list->SetVertexBuffer(0, s_storage->vertex_buffer);
		command_list->SetIndexBuffer(s_storage->index_buffer);
		command_list->SetPrimitiveTopology(rhi::PrimitiveTopology::kTriangleList);
		command_list->DrawIndexed(s_storage->index_buffer->GetDesc().size_in_bytes / s_storage->index_buffer->GetDesc().stride, 1, 0, 0, 0);
	}
}