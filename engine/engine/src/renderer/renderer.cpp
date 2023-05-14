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
		s_scene_data.camera_position = transform[3];
		s_render_data->render_target = render_target;
	}

	void Renderer::BeginScene(rhi::CommandList* command_list, const rhi::RenderTarget& render_target, const EditorCamera& camera)
	{
		s_scene_data.projection_matrix = camera.GetProjectionMatrx();
		s_scene_data.view_matrix = camera.GetViewMatrix();
		s_scene_data.view_projection_matrix = camera.GetViewProjectionMatrix();
		s_scene_data.camera_position = camera.GetPosition();
		s_render_data->render_target = render_target;
	}

	void Renderer::EndScene(rhi::CommandList* command_list)
	{

	}

	void Renderer::SetupLight(Light light)
	{
		s_scene_data.light = light;
	}

	void Renderer::DrawMesh(rhi::CommandList* command_list,const Material* material, rhi::Buffer* vertex_buffer, rhi::Buffer* index_buffer, const glm::mat4& model_matrix)
	{
		command_list->SetGraphicsPipeline(GetGraphicsPipeline(material->GetShader(), s_render_data->render_target));

		command_list->SetGraphicsDynamicConstantBuffer(rhi::Shader::GetBindIndex(rhi::Shader::kSceneDataName), s_scene_data);
		command_list->SetGraphicsDynamicConstantBuffer(rhi::Shader::GetBindIndex(rhi::Shader::kPerDrawConstantsName), model_matrix);

		auto& material_params_buffer = material->GetParamsBuffer();
		command_list->SetGraphicsDynamicConstantBuffer(rhi::Shader::GetBindIndex(rhi::Shader::kMaterialConstantsName), material_params_buffer.size(), material_params_buffer.data());

		command_list->SetVertexBuffer(0, vertex_buffer);
		command_list->SetIndexBuffer(index_buffer);

		command_list->SetPrimitiveTopology(rhi::PrimitiveTopology::kTriangleList);
		command_list->DrawIndexed(index_buffer->GetDesc().size_in_bytes / index_buffer->GetDesc().stride, 1, 0, 0, 0);
	}
	rhi::GraphicsPipeline* Renderer::GetGraphicsPipeline(const Shader* shader, const rhi::RenderTarget& render_target)
	{
		size_t hash = 0;

		//todo hash
		
		auto it = s_render_data->pso_cache.find(hash);
		if (it != s_render_data->pso_cache.end())
		{
			return it->second;
		}

		auto result = s_render_data->pso_cache.emplace(hash, CreateGraphicsPipeline(shader, render_target));
		return result.first->second;
	}

	rhi::GraphicsPipelineHandle Renderer::CreateGraphicsPipeline(const Shader* shader, const rhi::RenderTarget& render_target)
	{
		rhi::Device* device = Application::Get().GetDevice();

		std::vector<rhi::VertexAttributeDesc> vertex_attributes =
		{
			{ "POSITION",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,position),false},
			{ "NORMAL",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,normal),false },
			{ "TANGENT",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,tangent),false },
			{ "BINORMAL",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,bitangent),false },
			{ "TEXCOORD",0,rhi::Format::RG32_FLOAT,0,offsetof(Vertex,texcoord),false},
		};

		bool has_volatile_tex_range = false;
		uint32_t index = 3;
		rhi::BindingLayout* binding_layout = new rhi::BindingLayout(shader->GetBindResources().size());
		std::vector<rhi::BindingParameter::DescriptorRange> texture_ranges;
		std::vector<rhi::BindingParameter::DescriptorRange> sampler_ranges;
		for (auto& bind_resource : shader->GetBindResources())
		{
			rhi::BindingParameter bind_param;

			switch (bind_resource.type)
			{
			case rhi::ShaderBindResourceType::kConstantBuffer:
				bind_param.InitAsConstantBufferView(bind_resource.bind_point, bind_resource.space);
				binding_layout->Add(rhi::Shader::GetBindIndex(bind_resource.name), bind_param);
				break;

			case rhi::ShaderBindResourceType::kTexture:
				if (texture_ranges.empty() 
					|| !(texture_ranges.back().base_shader_register + texture_ranges.back().num_descriptors == bind_resource.bind_point
						&& texture_ranges.back().register_space == bind_resource.space))
				{
					rhi::BindingParameter::DescriptorRange range;
					range.range_type = rhi::DescriptorRangeType::kShaderResourceView;
					range.base_shader_register = bind_resource.bind_point;

					if (bind_resource.bind_count == 0) // dynamic array
					{
						// 只允许存在一个volatile tex range
						LIGHT_ASSERT(has_volatile_tex_range == false,"只允许存在一个volatile tex range!")

						range.is_volatile = true;
						// todo
						range.num_descriptors = kMaxTextures;
						has_volatile_tex_range = true;
					}
					else
					{
						range.num_descriptors = bind_resource.bind_count;
					}
					range.register_space = bind_resource.space;
					texture_ranges.emplace_back(range);
				}
				else
				{
					// 合并连续纹理绑定
					texture_ranges.back().num_descriptors += bind_resource.bind_count;
				}
				break;
			case rhi::ShaderBindResourceType::kSampler:
				if (sampler_ranges.empty() || !(sampler_ranges.back().base_shader_register + sampler_ranges.back().num_descriptors == bind_resource.bind_point
					&& sampler_ranges.back().register_space == bind_resource.space))
				{
					rhi::BindingParameter::DescriptorRange range;
					range.range_type = rhi::DescriptorRangeType::kSampler;
					range.base_shader_register = bind_resource.bind_point;
					range.num_descriptors = bind_resource.bind_count;
					range.register_space = bind_resource.space;
					sampler_ranges.emplace_back(range);
				}
				else 
				{
					// 合并连续的Sampler
					sampler_ranges.back().num_descriptors += bind_resource.bind_count;
				}
				break;
			default:
				LIGHT_ASSERT(false, "暂时还不支持!");
				break;
			}
		}
		
		if (!texture_ranges.empty())
		{
			rhi::BindingParameter texture_param;
			texture_param.InitAsDescriptorTable(texture_ranges.size(), texture_ranges.data(), rhi::ShaderVisibility::kPixel);

			binding_layout->Add(index++, texture_param);
		}
		
		if (!sampler_ranges.empty())
		{
			rhi::BindingParameter sampler_param;
			sampler_param.InitAsDescriptorTable(sampler_ranges.size(), sampler_ranges.data(), rhi::ShaderVisibility::kPixel);

			binding_layout->Add(index++, sampler_param);
		}

		rhi::GraphicsPipelineDesc pso_desc;
		pso_desc.input_layout = device->CreateInputLayout(vertex_attributes);
		pso_desc.binding_layout = rhi::BindingLayoutHandle::Create(binding_layout);
		pso_desc.vs = shader->GetVS();
		pso_desc.ps = shader->GetPS();
		pso_desc.blend_state.render_target[0].blend_enable = true;
		pso_desc.blend_state.render_target[0].src_blend = rhi::BlendFactor::kSrcAlpha;
		pso_desc.blend_state.render_target[0].dest_blend = rhi::BlendFactor::kInvSrcAlpha;
		pso_desc.primitive_type = rhi::PrimitiveTopology::kTriangleList;

		return device->CreateGraphicsPipeline(pso_desc, render_target);
	}
}
