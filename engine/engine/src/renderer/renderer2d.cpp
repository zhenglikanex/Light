#include "engine/renderer/renderer2d.h"
#include "engine/core/application.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "engine/profile/timer.h"
#include "engine/log/log.h"

namespace light
{
	Renderer2D::Data* Renderer2D::s_renderer_data = nullptr;
	Renderer2D::SceneData Renderer2D::s_scene_data = {};

	void Renderer2D::Init()
	{
		s_renderer_data = new Data();

		std::vector<rhi::VertexAttributeDesc> vertex_attributes =
		{
			{ "POSITION",0,rhi::Format::RGB32_FLOAT,0,0u,false },
			{ "TEXCOORD",0,rhi::Format::RG32_FLOAT,0,12,false },
			{ "COLOR",0,rhi::Format::RGBA32_FLOAT,0,20,false },
			{ "COLOR",1,rhi::Format::R32_FLOAT,0,36,false },
			{ "COLOR",2,rhi::Format::R32_FLOAT,0,40,false }
		};

		auto device = Application::Get().GetDevice();
		auto render_target = Application::Get().GetRenderTarget();
		auto command_list = device->GetCommandList(rhi::CommandListType::kCopy);

		rhi::TextureDesc white_desc;
		white_desc.width = 1;
		white_desc.height = 1;
		white_desc.format = rhi::Format::RGBA8_UNORM;

		s_renderer_data->white_texture = device->CreateTexture(white_desc);

		uint32_t data = 0xffffffff;
		std::vector<rhi::TextureSubresourceData> texture_data(1);
		texture_data[0].data = (char*)&data;
		texture_data[0].data_size = sizeof(data);
		texture_data[0].row_pitch = 4;
		command_list->WriteTexture(s_renderer_data->white_texture, 0, 1, texture_data);

		rhi::BufferDesc vertex_desc;
		vertex_desc.type = rhi::BufferType::kVertex;
		vertex_desc.format = rhi::Format::RGB32_FLOAT;
		vertex_desc.size_in_bytes = s_renderer_data->vertices.size() * sizeof(QuadVertex);
		vertex_desc.stride = sizeof(QuadVertex);
		//vertex_desc.cpu_access = rhi::CpuAccess::kWrite;
		s_renderer_data->vertex_buffer = device->CreateBuffer(vertex_desc);

		uint32_t offset = 0;
		for (uint32_t index = 0; index < s_renderer_data->indices.size(); index += 6)
		{
			s_renderer_data->indices[index + 0] = 0 + offset * 4;
			s_renderer_data->indices[index + 1] = 1 + offset * 4;
			s_renderer_data->indices[index + 2] = 2 + offset * 4;

			s_renderer_data->indices[index + 3] = 2 + offset * 4;
			s_renderer_data->indices[index + 4] = 3 + offset * 4;
			s_renderer_data->indices[index + 5] = 0 + offset * 4;

			++offset;
		}

		rhi::BufferDesc index_desc;
		index_desc.format = rhi::Format::R32_UINT;
		index_desc.type = rhi::BufferType::kIndex;
		index_desc.size_in_bytes = sizeof(uint32_t) * s_renderer_data->indices.size();
		index_desc.stride = sizeof(uint32_t);
		s_renderer_data->index_buffer = device->CreateBuffer(index_desc);

		command_list->WriteBuffer(s_renderer_data->index_buffer, reinterpret_cast<uint8_t*>(s_renderer_data->indices.data()), index_desc.size_in_bytes);

		command_list->ExecuteCommandList();

		rhi::BindingParameter scene_data_param;
		scene_data_param.InitAsConstantBufferView(0);

		rhi::BindingParameter::DescriptorRange sampler_range;
		sampler_range.base_shader_register = 0;
		sampler_range.num_descriptors = 1;
		sampler_range.range_type = rhi::DescriptorRangeType::kSampler;

		rhi::BindingParameter sampler_param;
		sampler_param.InitAsDescriptorTable(1, &sampler_range);

		rhi::BindingParameter::DescriptorRange tex_range;
		tex_range.base_shader_register = 0;
		tex_range.num_descriptors = kMaxTextures;
		tex_range.range_type = rhi::DescriptorRangeType::kShaderResourceView;
		tex_range.is_volatile = true;

		rhi::BindingParameter tex_param;
		tex_param.InitAsDescriptorTable(1, &tex_range);

		// create tex_pso
		rhi::BindingLayout* tex_binding_layout = new rhi::BindingLayout(3);
		tex_binding_layout->Add(static_cast<uint32_t>(ParameterIndex::kSceneData), scene_data_param);
		tex_binding_layout->Add(static_cast<uint32_t>(ParameterIndex::kTextures), tex_param);
		tex_binding_layout->Add(static_cast<uint32_t>(ParameterIndex::kSampler), sampler_param);

		rhi::ShaderHandle texture_vertex_shader = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kVertex, "assets/shaders/texture.hlsl", "VS", "vs_5_1");
		rhi::ShaderHandle texture_pixel_shader = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kPixel, "assets/shaders/texture.hlsl", "PS", "ps_5_1");

		rhi::GraphicsPipelineDesc tex_pso_desc;
		tex_pso_desc.input_layout = device->CreateInputLayout(vertex_attributes);
		tex_pso_desc.binding_layout = rhi::BindingLayoutHandle::Create(tex_binding_layout);
		tex_pso_desc.vs = texture_vertex_shader;
		tex_pso_desc.ps = texture_pixel_shader;
		tex_pso_desc.blend_state.render_target[0].blend_enable = true;
		tex_pso_desc.blend_state.render_target[0].src_blend = rhi::BlendFactor::kSrcAlpha;
		tex_pso_desc.blend_state.render_target[0].dest_blend = rhi::BlendFactor::kInvSrcAlpha;

		
		tex_pso_desc.primitive_type = rhi::PrimitiveTopology::kTriangleList;
		s_renderer_data->texture_pso = device->CreateGraphicsPipeline(tex_pso_desc, render_target);

		rhi::SamplerDesc sampler_desc;
		sampler_desc.filter = rhi::SamplerFilter::kMIN_MAG_MIP_POINT;
		s_renderer_data->point_sampler = device->CreateSampler(sampler_desc);

		uint32_t white_tex_slot = 0;
		s_renderer_data->texture_slots[white_tex_slot] = s_renderer_data->white_texture;

		s_renderer_data->quad_vertex_positions[0] = { -0.5f,-0.5f,0.0f,1.0f };
		s_renderer_data->quad_vertex_positions[1] = { -0.5f,0.5f,0.0f,1.0f };
		s_renderer_data->quad_vertex_positions[2] = { 0.5f,0.5f,0.0f,1.0f };
		s_renderer_data->quad_vertex_positions[3] = { 0.5f,-0.5f,0.0f,1.0f };
	}

	void Renderer2D::Shutdown()
	{
		delete s_renderer_data;
		s_renderer_data = nullptr;
	}

	void Renderer2D::BeginScene(rhi::CommandList* command_list, const OrthographicCamera& camera)
	{
		s_scene_data.projection_matrix = camera.GetProjectionMatrix();
		s_scene_data.view_matrix = camera.GetViewMatrix();
		s_scene_data.view_projection_matrix = camera.GetViewProjectionMatrix();

		command_list->SetGraphicsPipeline(s_renderer_data->texture_pso);
		command_list->SetGraphicsDynamicConstantBuffer(static_cast<uint32_t>(ParameterIndex::kSceneData), s_scene_data);
		command_list->SetSampler(static_cast<uint32_t>(ParameterIndex::kSampler), 0, s_renderer_data->point_sampler);
		command_list->SetIndexBuffer(s_renderer_data->index_buffer);
		command_list->SetPrimitiveTopology(rhi::PrimitiveTopology::kTriangleList);

		s_renderer_data->batch_quad_count = 0;
		s_renderer_data->texture_slot_index = 1;
	}

	void Renderer2D::EndScene(rhi::CommandList* command_list)
	{
		Flush(command_list);
	}

	void Renderer2D::Flush(rhi::CommandList* command_list)
	{
		command_list->SetDynamicVertexBuffer(0, s_renderer_data->vertices.data(), s_renderer_data->batch_quad_count * sizeof(QuadVertex) * 4, sizeof(QuadVertex));

		for (uint32_t index = 0; index < s_renderer_data->texture_slot_index; ++index)
		{
			command_list->SetShaderResourceView(static_cast<uint32_t>(ParameterIndex::kTextures), index, s_renderer_data->texture_slots[index]);
		}

		command_list->DrawIndexed(s_renderer_data->batch_quad_count * 6, 1, 0, 0, 0);

#ifdef STATISTICS
		++s_renderer_data->stats.draw_calls;
#endif

		for (uint32_t index = 1; index < s_renderer_data->texture_slot_index; ++index)
		{
			s_renderer_data->texture_slots[index] = nullptr;
		}
	}

	void Renderer2D::FlushAndReset(rhi::CommandList* command_list)
	{
		EndScene(command_list);

		s_renderer_data->batch_quad_count = 0;
		s_renderer_data->texture_slot_index = 1;
	}

	void Renderer2D::DrawQuad(rhi::CommandList* command_list, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(command_list, glm::vec3(position, 0.0f), size, color);
	}

	void Renderer2D::DrawQuad(rhi::CommandList* command_list, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		if (s_renderer_data->batch_quad_count >= kMaxBatchQuads)
		{
			FlushAndReset(command_list);
		}

		uint32_t index = s_renderer_data->batch_quad_count;

		float white_texture_slot = 0;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f),glm::vec3(size,1.0f));

		s_renderer_data->vertices[index * 4 + 0].position = transform * s_renderer_data->quad_vertex_positions[0];
		s_renderer_data->vertices[index * 4 + 0].texcoord = { 0.0f,1.0f };
		s_renderer_data->vertices[index * 4 + 0].color = color;
		s_renderer_data->vertices[index * 4 + 0].texture_index = white_texture_slot;
		s_renderer_data->vertices[index * 4 + 0].tiling_factor = 1;

		s_renderer_data->vertices[index * 4 + 1].position = transform * s_renderer_data->quad_vertex_positions[1];
		s_renderer_data->vertices[index * 4 + 1].texcoord = { 0.0f, 0.0f };
		s_renderer_data->vertices[index * 4 + 1].color = color;
		s_renderer_data->vertices[index * 4 + 1].texture_index = white_texture_slot;
		s_renderer_data->vertices[index * 4 + 1].tiling_factor = 1;

		s_renderer_data->vertices[index * 4 + 2].position = transform * s_renderer_data->quad_vertex_positions[2];
		s_renderer_data->vertices[index * 4 + 2].texcoord = { 1.0f, 0.0f };
		s_renderer_data->vertices[index * 4 + 2].color = color;
		s_renderer_data->vertices[index * 4 + 2].texture_index = white_texture_slot;
		s_renderer_data->vertices[index * 4 + 2].tiling_factor = 1;

		s_renderer_data->vertices[index * 4 + 3].position = transform * s_renderer_data->quad_vertex_positions[3];
		s_renderer_data->vertices[index * 4 + 3].texcoord = { 1.0f, 1.0f };
		s_renderer_data->vertices[index * 4 + 3].color = color;
		s_renderer_data->vertices[index * 4 + 3].texture_index = white_texture_slot;
		s_renderer_data->vertices[index * 4 + 3].tiling_factor = 1;

		++s_renderer_data->batch_quad_count;

#ifdef STATISTICS
		++s_renderer_data->stats.quad_count;
#endif

	}

	void Renderer2D::DrawQuad(rhi::CommandList* command_list, const glm::vec2& position, const glm::vec2& size, rhi::Texture* texture, float tiling_factor, glm::vec4 tint_color)
	{
		DrawQuad(command_list, glm::vec3(position, 0.0f), size, texture, tiling_factor, tint_color);
	}

	void Renderer2D::DrawQuad(rhi::CommandList* command_list, const glm::vec3& position, const glm::vec2& size, rhi::Texture* texture, float tiling_factor, glm::vec4 tint_color)
	{
		if (s_renderer_data->batch_quad_count >= kMaxBatchQuads)
		{
			FlushAndReset(command_list);
		}

		uint32_t index = s_renderer_data->batch_quad_count;

		uint32_t texture_slot = 0;

		for (uint32_t index = 0; index < s_renderer_data->texture_slot_index; ++index)
		{
			if (s_renderer_data->texture_slots[index].Get() == texture)
			{
				texture_slot = index;
				break;
			}
		}

		if (texture_slot == 0)
		{
			texture_slot = s_renderer_data->texture_slot_index++;
			s_renderer_data->texture_slots[texture_slot] = texture;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		s_renderer_data->vertices[index * 4 + 0].position = transform * s_renderer_data->quad_vertex_positions[0];
		s_renderer_data->vertices[index * 4 + 0].texcoord = { 0.0f,1.0f };
		s_renderer_data->vertices[index * 4 + 0].color = tint_color;
		s_renderer_data->vertices[index * 4 + 0].texture_index = texture_slot;
		s_renderer_data->vertices[index * 4 + 0].tiling_factor = tiling_factor;

		s_renderer_data->vertices[index * 4 + 1].position = transform * s_renderer_data->quad_vertex_positions[1];
		s_renderer_data->vertices[index * 4 + 1].texcoord = { 0.0f, 0.0f };
		s_renderer_data->vertices[index * 4 + 1].color = tint_color;
		s_renderer_data->vertices[index * 4 + 1].texture_index = texture_slot;
		s_renderer_data->vertices[index * 4 + 1].tiling_factor = tiling_factor;

		s_renderer_data->vertices[index * 4 + 2].position = transform * s_renderer_data->quad_vertex_positions[2];
		s_renderer_data->vertices[index * 4 + 2].texcoord = { 1.0f, 0.0f };
		s_renderer_data->vertices[index * 4 + 2].color = tint_color;
		s_renderer_data->vertices[index * 4 + 2].texture_index = texture_slot;
		s_renderer_data->vertices[index * 4 + 2].tiling_factor = tiling_factor;

		s_renderer_data->vertices[index * 4 + 3].position = transform * s_renderer_data->quad_vertex_positions[3];
		s_renderer_data->vertices[index * 4 + 3].texcoord = { 1.0f, 1.0f };
		s_renderer_data->vertices[index * 4 + 3].color = tint_color;
		s_renderer_data->vertices[index * 4 + 3].texture_index = texture_slot;
		s_renderer_data->vertices[index * 4 + 3].tiling_factor = tiling_factor;

		++s_renderer_data->batch_quad_count;

#ifdef STATISTICS
		++s_renderer_data->stats.quad_count;
#endif
	}

	void Renderer2D::DrawRotationQuad(rhi::CommandList* command_list, const glm::vec2& position, float rotation, const glm::vec2& size, const glm::vec4& color)
	{
		DrawRotationQuad(command_list, glm::vec3(position, 0.0f), rotation, size, color);
	}

	void Renderer2D::DrawRotationQuad(rhi::CommandList* command_list, const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& color)
	{
		if (s_renderer_data->batch_quad_count >= kMaxBatchQuads)
		{
			FlushAndReset(command_list);
		}

		uint32_t index = s_renderer_data->batch_quad_count;

		float white_texture_slot = 0;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1.0))
			* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		s_renderer_data->vertices[index * 4 + 0].position = transform * s_renderer_data->quad_vertex_positions[0];
		s_renderer_data->vertices[index * 4 + 0].texcoord = { 0.0f,1.0f };
		s_renderer_data->vertices[index * 4 + 0].color = color;
		s_renderer_data->vertices[index * 4 + 0].texture_index = white_texture_slot;
		s_renderer_data->vertices[index * 4 + 0].tiling_factor = white_texture_slot;
		s_renderer_data->vertices[index * 4 + 0].tiling_factor = 1;

		s_renderer_data->vertices[index * 4 + 1].position = transform * s_renderer_data->quad_vertex_positions[1];
		s_renderer_data->vertices[index * 4 + 1].texcoord = { 0.0f, 0.0f };
		s_renderer_data->vertices[index * 4 + 1].color = color;
		s_renderer_data->vertices[index * 4 + 1].texture_index = white_texture_slot;
		s_renderer_data->vertices[index * 4 + 1].tiling_factor = 1;

		s_renderer_data->vertices[index * 4 + 2].position = transform * s_renderer_data->quad_vertex_positions[2];
		s_renderer_data->vertices[index * 4 + 2].texcoord = { 1.0f, 0.0f };
		s_renderer_data->vertices[index * 4 + 2].color = color;
		s_renderer_data->vertices[index * 4 + 2].texture_index = white_texture_slot;
		s_renderer_data->vertices[index * 4 + 2].tiling_factor = 1;

		s_renderer_data->vertices[index * 4 + 3].position = transform * s_renderer_data->quad_vertex_positions[3];
		s_renderer_data->vertices[index * 4 + 3].texcoord = { 1.0f, 1.0f };
		s_renderer_data->vertices[index * 4 + 3].color = color;
		s_renderer_data->vertices[index * 4 + 3].texture_index = white_texture_slot;
		s_renderer_data->vertices[index * 4 + 3].tiling_factor = 1;

		++s_renderer_data->batch_quad_count;

#ifdef STATISTICS
		++s_renderer_data->stats.quad_count;
#endif
	}

	void Renderer2D::DrawRotationQuad(rhi::CommandList* command_list, const glm::vec2& position, float rotation, const glm::vec2& size, rhi::Texture* texture, float tiling_factor, glm::vec4 tint_color)
	{
		DrawRotationQuad(command_list, glm::vec3(position, 0.0f), rotation, size, texture, tiling_factor, tint_color);
	}

	void Renderer2D::DrawRotationQuad(rhi::CommandList* command_list, const glm::vec3& position, float rotation, const glm::vec2& size, rhi::Texture* texture, float tiling_factor, glm::vec4 tint_color)
	{
		if (s_renderer_data->batch_quad_count >= kMaxBatchQuads)
		{
			FlushAndReset(command_list);
		}

		uint32_t index = s_renderer_data->batch_quad_count;

		uint32_t texture_slot = 0;

		for (uint32_t index = 0; index < s_renderer_data->texture_slot_index; ++index)
		{
			if (s_renderer_data->texture_slots[index].Get() == texture)
			{
				texture_slot = index;
				break;
			}
		}

		if (texture_slot == 0)
		{
			texture_slot = s_renderer_data->texture_slot_index++;
			s_renderer_data->texture_slots[texture_slot] = texture;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1.0))
			* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		s_renderer_data->vertices[index * 4 + 0].position = transform * s_renderer_data->quad_vertex_positions[0];
		s_renderer_data->vertices[index * 4 + 0].texcoord = { 0.0f,1.0f };
		s_renderer_data->vertices[index * 4 + 0].color = tint_color;
		s_renderer_data->vertices[index * 4 + 0].texture_index = texture_slot;
		s_renderer_data->vertices[index * 4 + 0].tiling_factor = tiling_factor;

		s_renderer_data->vertices[index * 4 + 1].position = transform * s_renderer_data->quad_vertex_positions[1];
		s_renderer_data->vertices[index * 4 + 1].texcoord = { 0.0f, 0.0f };
		s_renderer_data->vertices[index * 4 + 1].color = tint_color;
		s_renderer_data->vertices[index * 4 + 1].texture_index = texture_slot;
		s_renderer_data->vertices[index * 4 + 1].tiling_factor = tiling_factor;

		s_renderer_data->vertices[index * 4 + 2].position = transform * s_renderer_data->quad_vertex_positions[2];
		s_renderer_data->vertices[index * 4 + 2].texcoord = { 1.0f, 0.0f };
		s_renderer_data->vertices[index * 4 + 2].color = tint_color;
		s_renderer_data->vertices[index * 4 + 2].texture_index = texture_slot;
		s_renderer_data->vertices[index * 4 + 2].tiling_factor = tiling_factor;

		s_renderer_data->vertices[index * 4 + 3].position = transform * s_renderer_data->quad_vertex_positions[3];
		s_renderer_data->vertices[index * 4 + 3].texcoord = { 1.0f, 1.0f };
		s_renderer_data->vertices[index * 4 + 3].color = tint_color;
		s_renderer_data->vertices[index * 4 + 3].texture_index = texture_slot;
		s_renderer_data->vertices[index * 4 + 3].tiling_factor = tiling_factor;

		++s_renderer_data->batch_quad_count;

#ifdef STATISTICS
		++s_renderer_data->stats.quad_count;
#endif
	}

	void Renderer2D::ResetStats()
	{
		s_renderer_data->stats.draw_calls = 0;
		s_renderer_data->stats.quad_count = 0;
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_renderer_data->stats;
	}

	void Renderer2D::ResetStats()
	{
		s_renderer_data->stats.draw_calls = 0;
		s_renderer_data->stats.quad_count = 0;
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_renderer_data->stats;
	}
}