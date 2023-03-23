#include "engine/application.h"
#include "engine/layer/layer.h"
#include "engine/event/key_event.h"
#include "engine/log/log.h"
#include "engine/input/input.h"
#include "engine/rhi/device.h"
#include "engine/rhi/buffer.h"
#include "engine/rhi/binding_layout.h"
#include "engine/rhi/command_list.h"
#include "engine/renderer/renderer.h"
#include "glm/glm.hpp"
#include "imgui.h"

using namespace light;
using namespace rhi;

class ExampleLayer : public Layer
{
	virtual void OnUpdate() override
	{
		LOG_GAME_INFO("Key 1 Is Pressed 我 ： {}",Input::IsKeyPressed(Input::Key::KEY_1));
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("hello light");
		ImGui::Button("hello light");
		ImGui::End();
	}

	virtual void OnEvent(const Event& e) override
	{

	}
};

class SandboxApplication : public Application
{
public:
	SandboxApplication()
		: Application()
	{
		PushOverlayLayer(new ExampleLayer());

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

		auto command_list = GetDevice()->GetCommandList(CommandListType::kCopy);

		rhi::BufferDesc vertex_desc;
		vertex_desc.type = rhi::BufferType::kVertex;
		vertex_desc.format = rhi::Format::RGB32_FLOAT;
		vertex_desc.size_in_bytes = sizeof(glm::vec3) * 3;
		vertex_desc.stride = sizeof(glm::vec3);
		vertex_buffer_ = GetDevice()->CreateBuffer(vertex_desc);

		command_list->WriteBuffer(vertex_buffer_, reinterpret_cast<uint8_t*>(vertexs.data()), vertex_desc.size_in_bytes);

		BufferDesc index_desc;
		index_desc.format = Format::R16_UINT;
		index_desc.type = BufferType::kIndex;
		index_desc.size_in_bytes = sizeof(uint16_t) * 3;
		index_desc.stride = sizeof(uint16_t);
		index_buffer_ = GetDevice()->CreateBuffer(index_desc);

		command_list->WriteBuffer(index_buffer_, reinterpret_cast<uint8_t*>(indices.data()), index_desc.size_in_bytes);

		command_list->ExecuteCommandList();

		BindingParameter scene_data_param;
		scene_data_param.InitAsConstants(sizeof(Renderer::SceneData) / 4, 0);

		BindingLayout* binding_layout = new BindingLayout(1);
		binding_layout->Add(0, scene_data_param);

		GraphicsPipelineDesc pso_desc;
		pso_desc.input_layout = GetDevice()->CreateInputLayout(std::move(vertex_attributes));
		pso_desc.binding_layout = BindingLayoutHandle::Create(binding_layout);
		pso_desc.vs = GetDevice()->CreateShader(ShaderType::kVertex, "assets/shaders/color.hlsl", "VS", "vs_5_0");
		pso_desc.ps = GetDevice()->CreateShader(ShaderType::kPixel, "assets/shaders/color.hlsl", "PS", "ps_5_0");
		pso_desc.primitive_type = PrimitiveTopology::kTriangleList;

		RenderTarget render_target = GetSwapChain()->GetRenderTarget();

		pso_ = GetDevice()->CreateGraphicsPipeline(pso_desc, render_target);
	}

	void OnRender(const RenderTarget& render_target) override
	{
		if (Input::IsKeyPressed(Input::Key::KEY_LEFT))
		{
			glm::vec3 pos = GetMainCamera().GetPosition();
			pos += glm::vec3(-0.01, 0, 0);
			GetMainCamera().SetPosition(pos);
		}
		else if(Input::IsKeyPressed(Input::Key::KEY_RIGHT))
		{
			glm::vec3 pos = GetMainCamera().GetPosition();
			pos += glm::vec3(0.01, 0, 0);
			GetMainCamera().SetPosition(pos);
		}
		else if (Input::IsKeyPressed(Input::Key::KEY_UP))
		{
			glm::vec3 pos = GetMainCamera().GetPosition();
			pos += glm::vec3(0, 0.01, 0);
			GetMainCamera().SetPosition(pos);
		}
		else if (Input::IsKeyPressed(Input::Key::KEY_DOWN))
		{
			glm::vec3 pos = GetMainCamera().GetPosition();
			pos += glm::vec3(0, -0.01, 0);
			GetMainCamera().SetPosition(pos);
		}
		else if (Input::IsKeyPressed(Input::Key::KEY_R))
		{
			float rotation = GetMainCamera().GetRotation();
			GetMainCamera().SetRotation(rotation + 0.1);
		}
		
		GetRenderer()->Submit(pso_, vertex_buffer_, index_buffer_);
	}
private:
	BufferHandle vertex_buffer_;
	BufferHandle index_buffer_;
	GraphicsPipelineHandle pso_;
};

light::Application* light::CreateApplication()
{
	return new SandboxApplication();
}
