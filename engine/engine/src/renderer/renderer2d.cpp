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
			{ "POSITION",0,rhi::Format::RGB32_FLOAT,0,0u,false},
			{ "TEXCOORD",0,rhi::Format::RG32_FLOAT,0,12,false}
		};

		auto device = Application::Get().GetDevice();
		auto render_target = Application::Get().GetRenderTarget();
		auto command_list = device->GetCommandList(rhi::CommandListType::kCopy);

		rhi::TextureDesc white_desc;
		white_desc.width = 1;
		white_desc.height = 1;
		white_desc.format = rhi::Format::RGBA8_UNORM;
		
		s_storage->white_texture = device->CreateTexture(white_desc);

		uint32_t data = 0xffffffff;
		std::vector<rhi::TextureSubresourceData> texture_data(1);
		texture_data[0].data = (char*)&data;
		texture_data[0].data_size = sizeof(data);
		texture_data[0].row_pitch = 4;
		command_list->WriteTexture(s_storage->white_texture, 0, 1, texture_data);

		std::array<float, 4 * 5> vertices
		{
			-0.5f, -0.5f, 0.0f,0.0f,1.0f,
			-0.5f, +0.5f, 0.0f,0.0f,0.0f,
			+0.5f, +0.5f, 0.0f,1.0f,0.0f,
			+0.5f, -0.5f, 0.0f,1.0f,1.0f,
		};

		std::array<uint16_t, 6> indices
		{
			0,1,2,
			0,2,3,
		};

		rhi::BufferDesc vertex_desc;
		vertex_desc.type = rhi::BufferType::kVertex;
		vertex_desc.format = rhi::Format::RGB32_FLOAT;
		vertex_desc.size_in_bytes = vertices.size() * sizeof(float);
		vertex_desc.stride = sizeof(float) * 5;
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

		rhi::BindingParameter color_param;
		color_param.InitAsConstants(sizeof(glm::vec4) / 4, 2);

		rhi::BindingParameter::DescriptorRange sampler_range;
		sampler_range.base_shader_register = 0;
		sampler_range.num_descriptors = 1;
		sampler_range.range_type = rhi::DescriptorRangeType::kSampler;

		rhi::BindingParameter sampler_param;
		sampler_param.InitAsDescriptorTable(1, &sampler_range);

		rhi::BindingParameter::DescriptorRange tex_range;
		tex_range.base_shader_register = 0;
		tex_range.num_descriptors = 1;
		tex_range.range_type = rhi::DescriptorRangeType::kShaderResourceView;

		rhi::BindingParameter tex_param;
		tex_param.InitAsDescriptorTable(1, &tex_range);

		// create tex_pso
		rhi::BindingLayout* tex_binding_layout = new rhi::BindingLayout(3);
		tex_binding_layout->Add(static_cast<uint32_t>(ParameterIndex::kSceneData), scene_data_param);
		tex_binding_layout->Add(static_cast<uint32_t>(ParameterIndex::kModelMatrix), model_matrix_param);
		tex_binding_layout->Add(static_cast<uint32_t>(ParameterIndex::kColor),color_param);
		tex_binding_layout->Add(static_cast<uint32_t>(ParameterIndex::kTexture), tex_param);
		tex_binding_layout->Add(static_cast<uint32_t>(ParameterIndex::kSampler), sampler_param);

		rhi::ShaderHandle texture_vertex_shader = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kVertex, "assets/shaders/texture.hlsl", "VS", "vs_5_0");
		rhi::ShaderHandle texture_pixel_shader = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kPixel, "assets/shaders/texture.hlsl", "PS", "ps_5_0");

		rhi::GraphicsPipelineDesc tex_pso_desc;
		tex_pso_desc.input_layout = device->CreateInputLayout(vertex_attributes);
		tex_pso_desc.binding_layout = rhi::BindingLayoutHandle::Create(tex_binding_layout);
		tex_pso_desc.vs = texture_vertex_shader;
		tex_pso_desc.ps = texture_pixel_shader;
		tex_pso_desc.primitive_type = rhi::PrimitiveTopology::kTriangleList;
		s_storage->texture_pso = device->CreateGraphicsPipeline(tex_pso_desc, render_target);

		rhi::SamplerDesc sampler_desc;
		sampler_desc.filter = rhi::SamplerFilter::kMIN_MAG_MIP_POINT;
		s_storage->point_sampler = device->CreateSampler(sampler_desc);
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

		command_list->SetGraphicsPipeline(s_storage->texture_pso);
		command_list->SetGraphicsDynamicConstantBuffer(static_cast<uint32_t>(ParameterIndex::kSceneData), s_scene_data);
		command_list->SetSampler(static_cast<uint32_t>(ParameterIndex::kSampler), 0, s_storage->point_sampler);
		command_list->SetVertexBuffer(0, s_storage->vertex_buffer);
		command_list->SetIndexBuffer(s_storage->index_buffer);
		command_list->SetPrimitiveTopology(rhi::PrimitiveTopology::kTriangleList);
	}

	void Renderer2D::EndScene()
	{

	}

	void Renderer2D::DrawQuad(rhi::CommandList* command_list, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(command_list, glm::vec3(position, 0.0f), size, color);
	}

	void Renderer2D::DrawQuad(rhi::CommandList* command_list, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		command_list->SetGraphics32BitConstants(static_cast<uint32_t>(ParameterIndex::kModelMatrix), model_matrix);
		command_list->SetGraphics32BitConstants(static_cast<uint32_t>(ParameterIndex::kColor), color);
		command_list->SetShaderResourceView(static_cast<uint32_t>(ParameterIndex::kTexture), 0, s_storage->white_texture);
		command_list->DrawIndexed(s_storage->index_buffer->GetDesc().size_in_bytes / s_storage->index_buffer->GetDesc().stride, 1, 0, 0, 0);
	}

	void Renderer2D::DrawQuad(rhi::CommandList* command_list, const glm::vec2& position, const glm::vec2& size, rhi::Texture* texture)
	{
		DrawQuad(command_list, glm::vec3(position, 0.0f), size, texture);
	}

	void Renderer2D::DrawQuad(rhi::CommandList* command_list, const glm::vec3& position, const glm::vec2& size, rhi::Texture* texture)
	{
		glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		command_list->SetGraphics32BitConstants(static_cast<uint32_t>(ParameterIndex::kModelMatrix), model_matrix);
		command_list->SetGraphics32BitConstants(static_cast<uint32_t>(ParameterIndex::kColor), glm::vec4(1.0f));
		command_list->SetShaderResourceView(static_cast<uint32_t>(ParameterIndex::kTexture), 0, texture);
		command_list->DrawIndexed(s_storage->index_buffer->GetDesc().size_in_bytes / s_storage->index_buffer->GetDesc().stride, 1, 0, 0, 0);
	}
}