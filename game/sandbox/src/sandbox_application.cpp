#include "engine/light.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace light;
using namespace rhi;

class ExampleLayer : public Layer
{
public:
	ExampleLayer()
		: camera_controller_(800.0f/450.0f,false)
	{
		shader_library_.Load("color",ShaderType::kVertex, "assets/shaders/color.hlsl");
		shader_library_.Load("color",ShaderType::kPixel, "assets/shaders/color.hlsl");

		std::vector<rhi::VertexAttributeDesc> vertex_attributes =
		{
			{"POSITION",0,rhi::Format::RGB32_FLOAT,0,0u,false}
		};

		std::vector<glm::vec3> vertexs
		{
			{ -0.5f, -0.5f, 0.5f },
			{ -0.5f, +0.5f, 0.5f },
			{ +0.5f, +0.5f, 0.5f },
		};

		std::vector<uint16_t> indices
		{
			0,1,2
		};

		auto device = Application::Get().GetDevice();
		auto command_list = device->GetCommandList(CommandListType::kCopy);

		rhi::BufferDesc vertex_desc;
		vertex_desc.type = rhi::BufferType::kVertex;
		vertex_desc.format = rhi::Format::RGB32_FLOAT;
		vertex_desc.size_in_bytes = sizeof(glm::vec3) * 3;
		vertex_desc.stride = sizeof(glm::vec3);
		vertex_buffer_ = device->CreateBuffer(vertex_desc);

		command_list->WriteBuffer(vertex_buffer_, reinterpret_cast<uint8_t*>(vertexs.data()), vertex_desc.size_in_bytes);

		BufferDesc index_desc;
		index_desc.format = Format::R16_UINT;
		index_desc.type = BufferType::kIndex;
		index_desc.size_in_bytes = sizeof(uint16_t) * 3;
		index_desc.stride = sizeof(uint16_t);
		index_buffer_ = device->CreateBuffer(index_desc);

		command_list->WriteBuffer(index_buffer_, reinterpret_cast<uint8_t*>(indices.data()), index_desc.size_in_bytes);

		command_list->ExecuteCommandList();

		BindingParameter scene_data_param;
		scene_data_param.InitAsConstantBufferView(0);

		BindingParameter model_matrix_param;
		model_matrix_param.InitAsConstants(sizeof(glm::mat4) / 4, 1);

		BindingParameter material_param;
		material_param.InitAsConstants(sizeof(glm::vec4) / 4, 2);

		BindingLayout* binding_layout = new BindingLayout(1);
		binding_layout->Add(static_cast<uint32_t>(Renderer::ParameterIndex::kSceneData), scene_data_param);
		binding_layout->Add(static_cast<uint32_t>(Renderer::ParameterIndex::kModelMatrix), model_matrix_param);
		binding_layout->Add(static_cast<uint32_t>(Renderer::ParameterIndex::kMaterial), material_param);

		GraphicsPipelineDesc pso_desc;
		pso_desc.input_layout = device->CreateInputLayout(std::move(vertex_attributes));
		pso_desc.binding_layout = BindingLayoutHandle::Create(binding_layout);
		pso_desc.vs = shader_library_.Get("color",rhi::ShaderType::kVertex);
		pso_desc.ps = shader_library_.Get("color", rhi::ShaderType::kPixel);
		pso_desc.primitive_type = PrimitiveTopology::kTriangleList;

		RenderTarget render_target = Application::Get().GetSwapChain()->GetRenderTarget();

		pso_ = device->CreateGraphicsPipeline(pso_desc, render_target);
	}

	virtual void OnUpdate(const Timestep& ts) override
	{
		//LOG_ENGINE_INFO("time : {}s {}ms", ts.GetSeconds(), ts.GetMilliseconds());

		camera_controller_.OnUpdate(ts);

		Application::Get().GetRenderer()->BeginScene(camera_controller_.GetCamera());

		glm::vec3 pos(-0.5, 0, 0);
		for (int i = 0; i < 10; ++i)
		{
			pos.x += 0.2;
			
			glm::mat4 model_matrix = glm::translate(glm::mat4(1), pos);
			
			Application::Get().GetRenderer()->Submit(pso_, vertex_buffer_, index_buffer_, model_matrix,glm::vec4(color_,1.0f));
		}

		Application::Get().GetRenderer()->EndScene();
	}

	virtual void OnImGuiRender(const Timestep& ts) override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("color",glm::value_ptr(color_));
		ImGui::End();
	}

	virtual void OnEvent(const Event& e) override
	{
		camera_controller_.OnEvent(e);
	}
private:
	ShaderLibrary shader_library_;
	BufferHandle vertex_buffer_;
	BufferHandle index_buffer_;
	GraphicsPipelineHandle pso_;
	OrthographicCameraController camera_controller_;
	glm::vec3 color_;
};

class SandboxApplication : public Application
{
public:
	SandboxApplication()
		: Application()
	{
	}

	void Init()
	{
		Application::Init();

		PushOverlayLayer(new ExampleLayer());
	}
private:
	
};

light::Application* light::CreateApplication()
{
	return new SandboxApplication();
}
