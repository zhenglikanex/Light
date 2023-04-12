#include "sandbox2d.h"

#include "spdlog/fmt/fmt.h"

using namespace light;
using namespace light::rhi;

#define PROFILE_SCOPE(name) Timer timer__FILE__##__LINE__(name,[this](std::tuple<std::string,float> result) { profile_results_.push_back(result); })

Sandbox2D::Sandbox2D()
	:camera_controller_(800.0f / 450.0f, false)
{	
	shader_library_.Load("color", ShaderType::kVertex, "assets/shaders/flat_color.hlsl");
	shader_library_.Load("color", ShaderType::kPixel, "assets/shaders/flat_color.hlsl");
}

void Sandbox2D::OnAttach()
{
	texture_ = texture_library_.LoadTexture("assets/textures/warchessMap_4.jpg");
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(const light::Timestep& ts)
{
	{
		PROFILE_SCOPE("CameraController OnUpdate");
		camera_controller_.OnUpdate(ts);
	}

	auto command_list = Application::Get().GetDevice()->GetCommandList(CommandListType::kDirect);
	auto render_target = Application::Get().GetRenderTarget();

	{
		PROFILE_SCOPE("Renderer Prep");

		command_list->SetRenderTarget(render_target);
		command_list->SetViewport(render_target.GetViewport());
		command_list->SetScissorRect({ 0,0,std::numeric_limits<int32_t>::max(),std::numeric_limits<int32_t>::max() });

		constexpr float clear_color[] = { 0.1, 0.1, 0.1, 1.0 };
		command_list->ClearTexture(render_target.GetAttachment(rhi::AttachmentPoint::kColor0).texture, clear_color);
		command_list->ClearDepthStencilTexture(render_target.GetAttachment(rhi::AttachmentPoint::kDepthStencil).texture,
			rhi::ClearFlags::kClearFlagDepth | rhi::ClearFlags::kClearFlagStencil, 1, 0);
	}

	Renderer2D::ResetStats();

	{
		PROFILE_SCOPE("Renderer Draw");
		Renderer2D::BeginScene(command_list, camera_controller_.GetCamera());

		static float rotation = 0;
		rotation += 10 * ts;

		Renderer2D::DrawQuad(command_list, glm::vec3(0.0f, 0.0f, 0.1f), glm::vec2(1.0f), glm::vec4(1.0f));
		Renderer2D::DrawQuad(command_list, glm::vec3(0.0f, 0.0f, -0.2f), glm::vec2(1.5f), texture_, 30);

		Renderer2D::DrawRotationQuad(command_list, glm::vec3(2.0f, 0.0f, 0.f), 0,glm::vec2(1.0f), texture_);

		Renderer2D::DrawQuad(command_list, glm::vec2(0.0f), glm::vec2(1.0f), { 1.0,1.0,0.5,1.0 });

		for (uint32_t i = 0; i < 3000; ++i)
		{
			Renderer2D::DrawQuad(command_list, glm::vec2(-1.0f + 0.01 * i,0), glm::vec2(1.f), { 1,0,0.0,1.0 });
		}
		
		Renderer2D::EndScene(command_list);
	}

	{
		PROFILE_SCOPE("Renderer ExecuteCommandList");
		command_list->ExecuteCommandList();
	}
}

void Sandbox2D::OnImGuiRender(const light::Timestep& ts)
{
	ImGui::Begin("Profile");
	for (auto& [name,dt] : profile_results_)
	{
		std::string str = fmt::format("{} : {}ms", name, dt);
		ImGui::Text(str.c_str());
	}
	
	Renderer2D::Statistics stats = Renderer2D::GetStats();
	ImGui::Text("draw call:%d", stats.draw_calls);
	ImGui::Text("quad count:%d", stats.quad_count);
	ImGui::Text("vertex count:%d", stats.GetVertexCount());
	ImGui::Text("index count:%d", stats.GetIndexCount());

	ImGui::End();
	profile_results_.clear();
}

void Sandbox2D::OnEvent(const light::Event& e)
{
	camera_controller_.OnEvent(e);
}
