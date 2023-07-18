#include "engine/renderer/renderer.h"

#include <ranges>

#include "engine/renderer/editor_camera.h"
#include "engine/renderer/mesh.h"
#include "engine/core/application.h"
#include "engine/asset/asset_manager.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace light
{
	Renderer::SceneData Renderer::s_scene_data = {};
	Renderer::RenderData* Renderer::s_render_data = nullptr;

	void Renderer::Init()
	{
		s_render_data = new RenderData();

		rhi::Device* device = Application::Get().GetDevice();
		auto command_list = device->GetCommandList(rhi::CommandListType::kCopy);

		// quad vertices
		{
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
			rhi::BufferHandle quad_vertex_buffer = device->CreateBuffer(vertex_buffer_desc);
			command_list->WriteBuffer(quad_vertex_buffer, reinterpret_cast<uint8_t*>(vertices.data()), vertex_buffer_desc.size_in_bytes);

			std::vector<rhi::VertexAttributeDesc> vertex_attributes = {
				{ "POSITION",0,rhi::Format::RGB32_FLOAT,0,offsetof(QuadVertex,position),false},
				{ "TEXCOORD",0,rhi::Format::RG32_FLOAT,0,offsetof(QuadVertex,texcoord),false }
			};

			rhi::InputLayoutHandle input_layout = device->CreateInputLayout(vertex_attributes);
			s_render_data->quad_vertex_buffer = MakeRef<VertexBuffer>(input_layout, quad_vertex_buffer);

			rhi::BufferDesc index_buffer_desc;
			index_buffer_desc.type = rhi::BufferType::kIndex;
			index_buffer_desc.format = rhi::Format::R32_UINT;
			index_buffer_desc.stride = sizeof(uint32_t);
			index_buffer_desc.size_in_bytes = sizeof(uint32_t) * indices.size();
			s_render_data->quad_index_buffer = device->CreateBuffer(index_buffer_desc);
			command_list->WriteBuffer(s_render_data->quad_index_buffer, reinterpret_cast<uint8_t*>(indices.data()), index_buffer_desc.size_in_bytes);
		}

		// cube vertices
		{
			float cube_vertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};

			std::array<uint32_t, 36> indices;
			for (int i = 0; i < 36; ++i)
			{
				indices[i] = i;
			}

			rhi::BufferDesc vertex_buffer_desc;
			vertex_buffer_desc.type = rhi::BufferType::kVertex;
			vertex_buffer_desc.stride = sizeof(float) * 3;
			vertex_buffer_desc.size_in_bytes = sizeof(float) * 3 * 36;
			rhi::BufferHandle cube_vertex_buffer = device->CreateBuffer(vertex_buffer_desc);
			command_list->WriteBuffer(cube_vertex_buffer, reinterpret_cast<uint8_t*>(cube_vertices), vertex_buffer_desc.size_in_bytes);

			std::vector<rhi::VertexAttributeDesc> vertex_attributes = {
				{ "POSITION",0,rhi::Format::RGB32_FLOAT,0,0,false},
			};

			rhi::InputLayoutHandle input_layout = device->CreateInputLayout(vertex_attributes);
			s_render_data->cube_vertex_buffer = MakeRef<VertexBuffer>(input_layout, cube_vertex_buffer);

			rhi::BufferDesc index_buffer_desc;
			index_buffer_desc.type = rhi::BufferType::kIndex;
			index_buffer_desc.format = rhi::Format::R32_UINT;
			index_buffer_desc.stride = sizeof(uint32_t);
			index_buffer_desc.size_in_bytes = sizeof(uint32_t) * indices.size();
			s_render_data->cube_index_buffer = device->CreateBuffer(index_buffer_desc);
			command_list->WriteBuffer(s_render_data->cube_index_buffer, reinterpret_cast<uint8_t*>(indices.data()), index_buffer_desc.size_in_bytes);
		}

		{
			rhi::SamplerDesc sampler_desc;
			sampler_desc.filter = rhi::SamplerFilter::kMIN_MAG_MIP_LINEAR;
			s_render_data->sampler = device->CreateSampler(sampler_desc);
		}

		// 内置textures
		{
			rhi::TextureDesc white_desc;
			white_desc.width = 1;
			white_desc.height = 1;
			white_desc.format = rhi::Format::RGBA8_UNORM;

			rhi::TextureHandle white_texture = device->CreateTexture(white_desc);
			uint32_t data = 0xffffffff;
			std::vector<rhi::TextureSubresourceData> texture_data(1);
			texture_data[0].data = (char*)&data;
			texture_data[0].data_size = sizeof(data);
			texture_data[0].row_pitch = 4;
			command_list->WriteTexture(white_texture, 0, 1, texture_data);

			s_render_data->builtin_textures.emplace("white", white_texture);
		}

		// shaders
		{
			s_render_data->equirectangular_to_cubemap_shader = AssetManager::LoadAsset<Shader>("shaders/equirectangular_to_cubemap.shader");
			s_render_data->equirectangular_to_cubemap_shader->SetCullMode(rhi::CullMode::kBack);
		}
		
		command_list->ExecuteCommandList();
	}

	void Renderer::Shutdown()
	{
		delete s_render_data;
		s_render_data = nullptr;
	}

	rhi::Texture* Renderer::GetBuiltinTexture(const std::string& name)
	{
		auto it = s_render_data->builtin_textures.find(name);
		if (it != s_render_data->builtin_textures.end())
		{
			return it->second;
		}

		return nullptr;
	}

	rhi::TextureHandle Renderer::CreateEnvironmentMap(rhi::CommandList* command_list,rhi::Texture* equirectangular)
	{
		rhi::Device* device = Application::Get().GetDevice();

		rhi::TextureDesc env_tex_desc;
		env_tex_desc.format = rhi::Format::RGBA32_FLOAT;
		env_tex_desc.dimension = rhi::TextureDimension::kTextureCube;
		env_tex_desc.width = 2048;
		env_tex_desc.height = 2048;
		env_tex_desc.is_render_target = true;

		rhi::ClearValue clear_value;
		clear_value.color[0] = 0;
		clear_value.color[1] = 0;
		clear_value.color[2] = 0;
		clear_value.color[3] = 1;

		rhi::TextureHandle env_tex = device->CreateTexture(env_tex_desc,&clear_value);

		glm::mat4 views[] =
		{
			glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f)),	// 右 +x
			glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f)),	// 左 -x
			glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f, 0.0f,  -1.0f)),	// 上 +y
			glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),	// 下 -y
			glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, 1.0f,  0.0f)),	// 前 +z
			glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, 1.0f,  0.0f)),	// 后 -z
		};

		struct Matrices
		{
			glm::mat4 projection;
			glm::mat4 view;
		};

		Matrices matrices;
		matrices.projection = glm::perspectiveLH_ZO(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

		for (int i = 0; i < 6; ++i)
		{
			rhi::RenderTarget render_target;
			render_target.AttachAttachment(rhi::AttachmentPoint::kColor0, env_tex, 0, i, 1);

			command_list->SetRenderTarget(render_target);
			command_list->SetViewport(render_target.GetViewport());
			command_list->SetScissorRect({ 0,0,std::numeric_limits<int32_t>::max(),std::numeric_limits<int32_t>::max() });
			command_list->ClearTexture(env_tex, 0, i, 1, env_tex->GetClearValue()->color);

			command_list->SetGraphicsPipeline(GetGraphicsPipeline(s_render_data->equirectangular_to_cubemap_shader, render_target,s_render_data->cube_vertex_buffer->GetInputLayout()));

			int32_t materix_data_index = s_render_data->equirectangular_to_cubemap_shader->FindConstantsBufferBindingIndex("cbMatrices");
			if (materix_data_index >= 0)
			{
				matrices.view = views[i];
				command_list->SetGraphicsDynamicConstantBuffer(materix_data_index, matrices);
			}

			auto table = s_render_data->equirectangular_to_cubemap_shader->FindTextureBindingTable("gEquirectangularMap");
			if (table)
			{
				command_list->SetShaderResourceView(table->index, table->offset, equirectangular);
			}

			for (const auto& sampler_binding_table : s_render_data->equirectangular_to_cubemap_shader->GetSamplerBindingTables() | std::views::values)
			{
				command_list->SetSampler(sampler_binding_table.index, sampler_binding_table.offset, s_render_data->sampler);
			}

			command_list->SetVertexBuffer(0, s_render_data->cube_vertex_buffer->GetBuffer());
			command_list->SetIndexBuffer(s_render_data->cube_index_buffer);

			command_list->SetPrimitiveTopology(rhi::PrimitiveTopology::kTriangleList);
			command_list->DrawIndexed(36, 1, 0, 0, 0);
		}

		return env_tex;
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
		s_scene_data.num_light = 0;
	}

	void Renderer::BeginRenderPass(rhi::CommandList* command_list, RenderPass* render_pass)
	{
		const rhi::RenderTarget& render_target = render_pass->GetResources().render_target;

		SetupRenderTarget(command_list, render_target);

		command_list->SetViewport(render_target.GetViewport());
		command_list->SetScissorRect({ 0,0,std::numeric_limits<int32_t>::max(),std::numeric_limits<int32_t>::max() });

		for (uint32_t i = 0; i < static_cast<uint32_t>(rhi::AttachmentPoint::kDepthStencil); ++i)
		{
			rhi::Texture* texture = render_target.GetAttachment(static_cast<rhi::AttachmentPoint>(i)).texture;
			if (texture)
			{
				LIGHT_ASSERT(texture->GetClearValue(), "not clear value!");

				command_list->ClearTexture(texture, texture->GetClearValue()->color);
			}
		}

		if (render_target.HasDepthAttachment())
		{
			rhi::Texture* texture = render_target.GetAttachment(rhi::AttachmentPoint::kDepthStencil).texture;

			LIGHT_ASSERT(texture->GetClearValue(), "not clear value!");

			const rhi::FormatInfo& format_info = rhi::GetFormatInfo(texture->GetDesc().format);
			if (format_info.has_stencil)
			{
				command_list->ClearDepthStencilTexture(texture, rhi::ClearFlags::kClearFlagDepth | rhi::ClearFlags::kClearFlagStencil,
					texture->GetClearValue()->depth_stencil.depth, texture->GetClearValue()->depth_stencil.stencil);
			}
			else
			{
				command_list->ClearDepthStencilTexture(texture, rhi::ClearFlags::kClearFlagDepth,
					texture->GetClearValue()->depth_stencil.depth, texture->GetClearValue()->depth_stencil.stencil);
			}
		}
	}

	void Renderer::EndRenderPass(rhi::CommandList* command_list, RenderPass* render_pass)
	{

	}

	void Renderer::SetupLight(Light light)
	{
		if (s_scene_data.num_light < kMaxLight)
		{
			s_scene_data.light[s_scene_data.num_light++] = light;
		}
	}

	void Renderer::SetupRenderTarget(rhi::CommandList* command_list, const rhi::RenderTarget& render_target)
	{
		command_list->SetRenderTarget(render_target);
		s_render_data->render_target = render_target;
	}

	void Renderer::Draw(
		rhi::CommandList* command_list,
		const Material* material,
		VertexBuffer* vertex_buffer,
		rhi::Buffer* index_buffer,
		const glm::mat4& model_matrix,
		uint32_t index_count,
		uint32_t base_vertex,
		uint32_t base_index)
	{
		const Shader* shader = material->GetShader();

		command_list->SetGraphicsPipeline(GetGraphicsPipeline(shader, s_render_data->render_target,vertex_buffer->GetInputLayout()));

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

		command_list->SetVertexBuffer(0, vertex_buffer->GetBuffer());
		command_list->SetIndexBuffer(index_buffer);

		command_list->SetPrimitiveTopology(rhi::PrimitiveTopology::kTriangleList);
		command_list->DrawIndexed(index_count, 1, base_index, base_vertex, 0);
	}

	void Renderer::DrawQuad(rhi::CommandList* command_list, const Shader* shader, glm::vec2 position, glm::vec2 scale)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(scale, 1.0f));

		command_list->SetGraphicsPipeline(GetGraphicsPipeline(shader, s_render_data->render_target,s_render_data->quad_vertex_buffer->GetInputLayout()));
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
			command_list->SetSampler(sampler_binding_table.index, sampler_binding_table.offset, s_render_data->sampler);
		}

		command_list->SetVertexBuffer(0, s_render_data->quad_vertex_buffer->GetBuffer());
		command_list->SetIndexBuffer(s_render_data->quad_index_buffer);

		command_list->SetPrimitiveTopology(rhi::PrimitiveTopology::kTriangleList);
		command_list->DrawIndexed(6, 1, 0, 0, 0);
	}

	void Renderer::DrawSkybox(rhi::CommandList* command_list, const Material* material)
	{
		const Shader* shader = material->GetShader();
		
		command_list->SetGraphicsPipeline(GetGraphicsPipeline(shader, s_render_data->render_target, s_render_data->cube_vertex_buffer->GetInputLayout()));

		int32_t matrices_index = shader->FindConstantsBufferBindingIndex("cbMatrices");
		if (matrices_index >= 0)
		{
			struct Matrices
			{
				glm::mat4 projection;
				glm::mat4 view;
			};

			Matrices matrices;
			matrices.projection = s_scene_data.projection_matrix;
			matrices.view = glm::mat4(glm::mat3(s_scene_data.view_matrix));
			
			command_list->SetGraphicsDynamicConstantBuffer(matrices_index,matrices);
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

		command_list->SetVertexBuffer(0, s_render_data->cube_vertex_buffer->GetBuffer());
		command_list->SetIndexBuffer(s_render_data->cube_index_buffer);

		command_list->SetPrimitiveTopology(rhi::PrimitiveTopology::kTriangleList);
		command_list->DrawIndexed(36, 1, 0, 0, 0);
	}

	rhi::GraphicsPipeline* Renderer::GetGraphicsPipeline(const Shader* shader, const rhi::RenderTarget& render_target, rhi::InputLayout* input_layout)
	{
		size_t hash = 0;

		light::HashCombine(hash, (uint64_t)shader->GetCullMode());
		HashCombine(hash, shader->GetBindResources().size());

		for (auto& bind_resource : shader->GetBindResources())
		{
			light::HashCombine(hash, bind_resource);
		}

		light::HashCombine(hash, render_target);

		// todo:是否遍历input_layout进行hash?
		HashCombine(hash, input_layout);

		auto it = s_render_data->pso_cache.find(hash);
		if (it != s_render_data->pso_cache.end())
		{
			return it->second;
		}

		auto result = s_render_data->pso_cache.emplace(hash, CreateGraphicsPipeline(shader, render_target, input_layout));
		return result.first->second;
	}

	rhi::GraphicsPipelineHandle Renderer::CreateGraphicsPipeline(const Shader* shader, const rhi::RenderTarget& render_target, rhi::InputLayout* input_layout)
	{
		rhi::Device* device = Application::Get().GetDevice();

		rhi::GraphicsPipelineDesc pso_desc;
		pso_desc.input_layout = input_layout;
		pso_desc.binding_layout = shader->GetBindingLayout();
		pso_desc.vs = shader->GetVS();
		pso_desc.ps = shader->GetPS();
		pso_desc.rasterizer_state.cull_mode = shader->GetCullMode();
		pso_desc.blend_state.render_target[0].blend_enable = true;
		pso_desc.blend_state.render_target[0].src_blend = rhi::BlendFactor::kSrcAlpha;
		pso_desc.blend_state.render_target[0].dest_blend = rhi::BlendFactor::kInvSrcAlpha;
		pso_desc.primitive_type = rhi::PrimitiveTopology::kTriangleList;
		pso_desc.depth_stencil_state.depth_enable = shader->IsDepthEnable();
		pso_desc.depth_stencil_state.depth_func = shader->GetDepthFunc();

		return device->CreateGraphicsPipeline(pso_desc, render_target);
	}
}
