#include "engine/renderer/renderer.h"

#include <ranges>

#include "engine/renderer/editor_camera.h"
#include "engine/renderer/mesh.h"
#include "engine/core/application.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace light
{
	Renderer::SceneData Renderer::s_scene_data = {};
	Renderer::RenderData* Renderer::s_render_data = nullptr;
	ShaderLibrary Renderer::s_shader_library = {};

	void Renderer::Init()
	{
		s_render_data = new RenderData();

		rhi::Device* device = Application::Get().GetDevice();

		auto command_list = device->GetCommandList(rhi::CommandListType::kCopy);

		std::array<QuadVertex, 4> vertices =
		{
			QuadVertex{ {-1,-1,0.1},{0,0} },
			QuadVertex{ {-1,1,0.1},{0,1} },
			QuadVertex{ {1,1,0.1},{1,1} },
			QuadVertex{ {1,-1,0.1},{1,0} },
		};

		std::array<uint32_t, 6> indices = { 0,1,2,2,3,0 };

		rhi::BufferDesc vertex_buffer_desc;
		vertex_buffer_desc.type = rhi::BufferType::kVertex;
		vertex_buffer_desc.stride = sizeof(QuadVertex);
		vertex_buffer_desc.size_in_bytes = sizeof(QuadVertex) * vertices.size();
		s_render_data->quad_vertex_buffer = device->CreateBuffer(vertex_buffer_desc);
		command_list->WriteBuffer(s_render_data->quad_vertex_buffer, reinterpret_cast<uint8_t*>(vertices.data()), vertex_buffer_desc.size_in_bytes);

		rhi::BufferDesc index_buffer_desc;
		index_buffer_desc.type = rhi::BufferType::kIndex;
		index_buffer_desc.format = rhi::Format::R32_UINT;
		index_buffer_desc.stride = sizeof(uint32_t);
		index_buffer_desc.size_in_bytes = sizeof(uint32_t) * indices.size();
		s_render_data->quad_index_buffer = device->CreateBuffer(index_buffer_desc);
		command_list->WriteBuffer(s_render_data->quad_index_buffer, reinterpret_cast<uint8_t*>(indices.data()), index_buffer_desc.size_in_bytes);

		command_list->ExecuteCommandList();

		rhi::SamplerDesc sampler_desc;
		sampler_desc.filter = rhi::SamplerFilter::kMIN_MAG_MIP_POINT;
		s_render_data->sampler = device->CreateSampler(sampler_desc);
	}

	void Renderer::Shutdown()
	{
		delete s_render_data;
		s_render_data = nullptr;
	}

	void Renderer::BeginScene(rhi::CommandList* command_list, const Camera& camera, const glm::mat4& transform)
	{
		s_scene_data.projection_matrix = camera.GetProjection();
		s_scene_data.view_matrix = glm::inverse(transform);
		s_scene_data.view_projection_matrix = s_scene_data.projection_matrix * s_scene_data.view_matrix;
		s_scene_data.camera_position = transform[3];
	}

	void Renderer::BeginScene(rhi::CommandList* command_list, const EditorCamera& camera)
	{
		s_scene_data.projection_matrix = camera.GetProjectionMatrx();
		s_scene_data.view_matrix = camera.GetViewMatrix();
		s_scene_data.view_projection_matrix = camera.GetViewProjectionMatrix();
		s_scene_data.camera_position = camera.GetPosition();
	}

	void Renderer::EndScene(rhi::CommandList* command_list)
	{

	}

	void Renderer::SetupLight(Light light)
	{
		s_scene_data.light = light;
	}

	void Renderer::SetupRenderTarget(rhi::CommandList* command_list, const rhi::RenderTarget& render_target)
	{
		command_list->SetRenderTarget(render_target);
		s_render_data->render_target = render_target;
	}

	void Renderer::Draw(
		rhi::CommandList* command_list,
		const Material* material,
		rhi::Buffer* vertex_buffer,
		rhi::Buffer* index_buffer,
		const glm::mat4& model_matrix,
		uint32_t index_count,
		uint32_t base_vertex,
		uint32_t base_index)
	{
		const Shader* shader = material->GetShader();

		command_list->SetGraphicsPipeline(GetGraphicsPipeline(shader, s_render_data->render_target, typeid(Vertex).hash_code()));

		int32_t scene_data_index = shader->FindConstantsBufferBindingIndex(rhi::Shader::kSceneDataName);
		if (scene_data_index >= 0)
		{
			command_list->SetGraphicsDynamicConstantBuffer(scene_data_index, s_scene_data);
		}

		int32_t model_matrix_index = shader->FindConstantsBufferBindingIndex(rhi::Shader::kPerDrawConstantsName);
		if (model_matrix_index >= 0)
		{
			command_list->SetGraphicsDynamicConstantBuffer(model_matrix_index, model_matrix);
		}

		int32_t material_index = shader->FindConstantsBufferBindingIndex(rhi::Shader::kMaterialConstantsName);
		if (material_index >= 0)
		{
			const auto& material_params_buffer = material->GetParamsBuffer();
			command_list->SetGraphicsDynamicConstantBuffer(material_index, material_params_buffer.size(), material_params_buffer.data());
		}

		for (auto& [name, texture] : material->GetTextures())
		{
			auto table = shader->FindTextureBindingTable(name);
			if (table)
			{
				command_list->SetShaderResourceView(table->index, table->offset, texture);
			}
		}

		for (const auto& sampler_binding_table : shader->GetSamplerBindingTables() | std::views::values)
		{
			command_list->SetSampler(sampler_binding_table.index, sampler_binding_table.offset, s_render_data->sampler);
		}

		command_list->SetVertexBuffer(0, vertex_buffer);
		command_list->SetIndexBuffer(index_buffer);

		command_list->SetPrimitiveTopology(rhi::PrimitiveTopology::kTriangleList);
		command_list->DrawIndexed(index_count, 1, base_index, base_vertex, 0);
	}

	void Renderer::DrawQuad(rhi::CommandList* command_list, const Shader* shader, glm::vec2 position, glm::vec2 scale)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(scale, 1.0f));

		command_list->SetGraphicsPipeline(GetGraphicsPipeline(shader, s_render_data->render_target, typeid(QuadVertex).hash_code()));
		int32_t model_matrix_index = shader->FindConstantsBufferBindingIndex(rhi::Shader::kPerDrawConstantsName);
		if (model_matrix_index >= 0)
		{
			command_list->SetGraphicsDynamicConstantBuffer(shader->FindConstantsBufferBindingIndex(rhi::Shader::kPerDrawConstantsName), transform);
		}

		int32_t material_index = shader->FindConstantsBufferBindingIndex(rhi::Shader::kMaterialConstantsName);
		if (material_index >= 0)
		{
			const auto& params_buffer = shader->GetParamsBuffer();
			command_list->SetGraphicsDynamicConstantBuffer(material_index, params_buffer.size(), params_buffer.data());
		}

		for (auto& [name, texture] : shader->GetTextures())
		{
			auto table = shader->FindTextureBindingTable(name);
			if (table)
			{
				command_list->SetShaderResourceView(table->index, table->offset, texture);
			}
		}

		for (const auto& sampler_binding_table : shader->GetSamplerBindingTables() | std::views::values)
		{
			command_list->SetSampler(sampler_binding_table.index, sampler_binding_table.offset,s_render_data->sampler);
		}

		command_list->SetVertexBuffer(0, s_render_data->quad_vertex_buffer);
		command_list->SetIndexBuffer(s_render_data->quad_index_buffer);

		command_list->SetPrimitiveTopology(rhi::PrimitiveTopology::kTriangleList);
		command_list->DrawIndexed(6, 1, 0, 0, 0);
	}

	rhi::GraphicsPipeline* Renderer::GetGraphicsPipeline(const Shader* shader, const rhi::RenderTarget& render_target, size_t vertex_type)
	{
		size_t hash = 0;

		HashCombine(hash, shader->GetBindResources().size());

		for(auto& bind_resource : shader->GetBindResources())
		{
			light::HashCombine(hash, bind_resource);
		}

		light::HashCombine(hash, render_target);
		HashCombine(hash, vertex_type);
		
		auto it = s_render_data->pso_cache.find(hash);
		if (it != s_render_data->pso_cache.end())
		{
			return it->second;
		}

		auto result = s_render_data->pso_cache.emplace(hash, CreateGraphicsPipeline(shader, render_target, vertex_type));
		return result.first->second;
	}

	rhi::GraphicsPipelineHandle Renderer::CreateGraphicsPipeline(const Shader* shader, const rhi::RenderTarget& render_target, size_t vertex_type)
	{
		rhi::Device* device = Application::Get().GetDevice();

		std::vector<rhi::VertexAttributeDesc> vertex_attributes;

		if (vertex_type == typeid(Vertex).hash_code())
		{
			vertex_attributes = {
				{ "POSITION",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,position),false},
				{ "NORMAL",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,normal),false },
				{ "TANGENT",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,tangent),false },
				{ "BINORMAL",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,bitangent),false },
				{ "TEXCOORD",0,rhi::Format::RG32_FLOAT,0,offsetof(Vertex,texcoord),false},
			};
		}
		else if (vertex_type == typeid(QuadVertex).hash_code())
		{
			vertex_attributes = {
				{ "POSITION",0,rhi::Format::RGB32_FLOAT,0,offsetof(QuadVertex,position),false},
				{ "TEXCOORD",0,rhi::Format::RG32_FLOAT,0,offsetof(QuadVertex,texcoord),false }
			};
		}
		else
		{
			LIGHT_ASSERT(false, "unkown vertex type!");
		}

		
		rhi::GraphicsPipelineDesc pso_desc;
		pso_desc.input_layout = device->CreateInputLayout(vertex_attributes);
		pso_desc.binding_layout = shader->CreateBindingLayout();
		pso_desc.vs = shader->GetVS();
		pso_desc.ps = shader->GetPS();
		pso_desc.blend_state.render_target[0].blend_enable = true;
		pso_desc.blend_state.render_target[0].src_blend = rhi::BlendFactor::kSrcAlpha;
		pso_desc.blend_state.render_target[0].dest_blend = rhi::BlendFactor::kInvSrcAlpha;
		pso_desc.primitive_type = rhi::PrimitiveTopology::kTriangleList;

		return device->CreateGraphicsPipeline(pso_desc, render_target);
	}
}
