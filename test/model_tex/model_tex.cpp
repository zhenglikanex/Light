#include "../rhi_base.h"
#include "rhi/sampler.h"

#define GLM_FORCE_LEFT_HANDED

#include <thread>
#include <chrono>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "../texture_loader.h"
#include "../mesh.h"

#undef max
#undef min

using namespace light;
using namespace light::rhi;

struct alignas(4) PerFrameConstants
{
	glm::mat4 proj_matrix;
	glm::mat4 view_matrix;
	glm::mat4 proj_view_matrix;
};

struct PerDrawConstants
{
	glm::mat4 model_matrix;
};


struct Vertex
{
	float x;
	float y;
	float z;
};

struct Color
{
	float r;
	float g;
	float b;
	float a;
};

class TestGame final : public Game
{
public:
	explicit TestGame(const GameParams& params)
		: Game(params)
	{
	}

	~TestGame() override = default;

	void OnResize(uint32_t width, uint32_t height) override
	{
		Game::OnResize(width, height);
	}

	bool OnInit() override
	{
		TextureLoader loader;
		tex_ = loader.LoadTexture(device_,"assets/Cube/Cube_BaseColor.png");

		GLTFReader reader(device_);

		if(reader.Load("assets/Cube/Cube.gltf"))
		{
			meshes_ = reader.GetMeshes();
		}

		auto command_queue = device_->GetCommandQueue(CommandListType::kDirect);
		auto command_list = command_queue->GetCommandList();

		std::vector<VertexAttributeDesc> vertex_attributes =
		{
			{"POSITION",0,Format::RGB32_FLOAT,0,0u,false},
			{"NORMAL",0,Format::RGB32_FLOAT,1,0u,false},
			{"TEXCOORD",0,Format::RG32_FLOAT,2,0u,false}
		};

		std::vector<Vertex> vertexs
		{
			{ -0.5f, -0.5f, 0.5f },
			{ -0.5f, +0.5f, 0.5f },
			{ +0.5f, +0.5f, 0.5f },
		};

		std::vector<uint16_t> indices
		{
			0,1,2
		};

		BufferDesc vertex_desc;
		vertex_desc.type = BufferType::kVertex;
		vertex_desc.format = Format::RGB32_FLOAT;
		vertex_desc.size_in_bytes = sizeof(Vertex) * 3;
		vertex_desc.stride = sizeof(Vertex);
		vertex_buffer_ = device_->CreateBuffer(vertex_desc);
		
		command_list->WriteBuffer(vertex_buffer_, reinterpret_cast<uint8_t*>(vertexs.data()), vertexs.size() * sizeof(Vertex));

		BufferDesc index_desc;
		index_desc.format = Format::R16_UINT;
		index_desc.type = BufferType::kIndex;
		index_desc.size_in_bytes = sizeof(uint16_t) * 3;
		index_desc.stride = sizeof(uint16_t);
		index_buffer_ = device_->CreateBuffer(index_desc);

		command_list->WriteBuffer(index_buffer_, reinterpret_cast<uint8_t*>(indices.data()), indices.size() * sizeof(uint16_t));

		TextureDesc depth_tex_desc;
		depth_tex_desc.format = Format::D24S8;
		depth_tex_desc.width = swap_chain_->GetWidth();
		depth_tex_desc.height = swap_chain_->GetHeight();

		depth_stencil_texture_ = device_->CreateTexture(depth_tex_desc);

		SamplerDesc sampler_desc;
		sampler_ = device_->CreateSampler(sampler_desc);

		BindingParameter parameter1;
		parameter1.InitAsConstants(sizeof(PerFrameConstants) / 4, 0);
		
		BindingParameter parameter2;
		parameter2.InitAsConstantBufferView(1);

		BindingParameter::DescriptorRange range;
		range.base_shader_register = 0;
		range.num_descriptors = 1;
		range.range_type = DescriptorRangeType::kSampler;

		BindingParameter parameter3;
		parameter3.InitAsDescriptorTable(1, &range);

		BindingParameter::DescriptorRange tex_range;
		tex_range.base_shader_register = 0;
		tex_range.num_descriptors = 1;
		tex_range.range_type = DescriptorRangeType::kShaderResourceView;
		BindingParameter parameter4;
		parameter4.InitAsDescriptorTable(1, &tex_range);
		
		auto* binding_layout = new BindingLayout(4);
		binding_layout->Add(0, parameter1);
		binding_layout->Add(1, parameter2);
		binding_layout->Add(2, parameter3);
		binding_layout->Add(3, parameter4);

		GraphicsPipelineDesc pso_desc;
		pso_desc.input_layout = device_->CreateInputLayout(std::move(vertex_attributes));
		pso_desc.binding_layout = BindingLayoutHandle::Create(binding_layout);
		pso_desc.vs = device_->CreateShader(ShaderType::kVertex, "shaders/cube.hlsl", "VS", "vs_5_0");
		pso_desc.ps = device_->CreateShader(ShaderType::kPixel, "shaders/cube.hlsl", "PS", "ps_5_0");
		pso_desc.primitive_type = PrimitiveTopology::kTriangleList;

		RenderTarget rt = swap_chain_->GetRenderTarget();
		rt.AttacthAttachment(AttachmentPoint::kDepthStencil, depth_stencil_texture_);

		pso_ = device_->CreateGraphicsPipeline(pso_desc,rt);

		command_list->ExecuteCommandList();

		command_queue->Flush();

		return true;
	}

	void OnUpdate(double dt) override
	{
		PerFrameConstants per_frame_constants;

		per_frame_constants.view_matrix = glm::lookAt(glm::vec3(2, 0, -10),glm::vec3(0),glm::vec3(0, 1, 0));
		per_frame_constants.proj_matrix = glm::perspective(glm::radians(60.0f), 1280.0f / 720.f, 1.f, 1000.0f);
		per_frame_constants.proj_view_matrix = per_frame_constants.proj_matrix * per_frame_constants.view_matrix;

		const auto command_list = device_->GetCommandList(CommandListType::kDirect);

		RenderTarget rt = swap_chain_->GetRenderTarget();
		rt.AttacthAttachment(AttachmentPoint::kDepthStencil, depth_stencil_texture_);

		command_list->SetRenderTarget(rt);
		command_list->SetViewport(rt.GetViewport());
		command_list->SetScissorRect({ 0,0,std::numeric_limits<int32_t>::max(),std::numeric_limits<int32_t>::max() });

		constexpr float clear_color[] = { 1.0, 0.0, 0.0, 1.0 };
		command_list->ClearTexture(rt.GetAttachment(AttachmentPoint::kColor0).texture,clear_color);

		command_list->ClearDepthStencilTexture(depth_stencil_texture_, ClearFlags::kClearFlagDepth | ClearFlags::kClearFlagStencil, 1, 0);

		command_list->SetGraphicsPipeline(pso_);

		command_list->SetGraphics32BitConstants(0, per_frame_constants);

		command_list->SetSampler(2, 0, sampler_);

		command_list->SetShaderResourceView(3, 0, tex_);

		/*command_list->SetPrimitiveTopology(PrimitiveTopology::kTriangleList);
		command_list->SetVertexBuffer(0, vertex_buffer_);
		command_list->SetIndexBuffer(index_buffer_);*/

		//command_list->DrawIndexed(3, 1, 0, 0, 0);

		DrawMesh(command_list);

		command_list->ExecuteCommandList();
		
		swap_chain_->Present();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	void DrawMesh(CommandList* command_list) const
	{
		for(auto& mesh : meshes_)
		{
			PerDrawConstants per_draw_constants;
			per_draw_constants.model_matrix = mesh.model_matrix;
			command_list->SetGraphicsDynamicConstantBuffer(1, per_draw_constants);

			command_list->SetPrimitiveTopology(mesh.primitive);
			command_list->SetVertexBuffers(mesh.vertex_buffers);
			command_list->SetIndexBuffer(mesh.index_buffer);
			command_list->DrawIndexed(mesh.GetIndexCount(), 1, 0, 0, 0);
		}
	}

	void OnRender(double dt) override
	{
		
	}
private:
	TextureHandle depth_stencil_texture_;
	GraphicsPipelineHandle pso_;
	BufferHandle vertex_buffer_;
	BufferHandle index_buffer_;
	std::vector<Mesh> meshes_;
	TextureHandle tex_;
	SamplerHandle sampler_;
};

int main()
{
	GameParams params;

	TestGame game(params);

	if(game.Initialize())
	{
		game.Run();
	}

	game.Shutdown();
	
	return 0;
}
