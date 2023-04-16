#include "test2d.h"

#include "spdlog/fmt/fmt.h"

using namespace light;
using namespace light::rhi;

Test2D::Test2D()
	:camera_controller_(800.0f / 450.0f, false)
{	
	shader_library_.Load("color", ShaderType::kVertex, "assets/shaders/flat_color.hlsl");
	shader_library_.Load("color", ShaderType::kPixel, "assets/shaders/flat_color.hlsl");
}

void Test2D::OnAttach()
{
	texture_ = texture_library_.LoadTexture("assets/textures/warchessMap_4.jpg");
}

void Test2D::OnDetach()
{

}

void Test2D::OnUpdate(const light::Timestep& ts)
{
	//LOG_ENGINE_INFO("TIME : {}ms", ts.GetMilliseconds());
	PROFILE_FUNCATION();

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

		for (float x = -5.0f; x < 5.0f; x += 0.1f)
		{
			for (float y = -5.0f; y < 5.0f; y += 0.1f)
			{
				Renderer2D::DrawQuad(command_list, glm::vec3(x, y, 0.1f), glm::vec2(0.085f), texture_);
			}
		}

		for (float x = -5.0f; x < 5.0f; x += 0.1f)
		{
			for (float y = -5.0f; y < 5.0f; y += 0.1f)
			{
				Renderer2D::DrawQuad(command_list, glm::vec3(x, y, 0), glm::vec2(0.085f), { (x + 5.0f) / 10.0f,(y + 5.0f) / 10.0f,0.0,0.8f });
			}
		}

		for (float x = -5.0f; x < 5.0f; x += 0.1f)
		{
			for (float y = -5.0f; y < 5.0f; y += 0.1f)
			{
				Renderer2D::DrawRotationQuad(command_list, glm::vec3(x, y, 0),rotation, glm::vec2(0.085f), { (x + 5.0f) / 10.0f,(y + 5.0f) / 10.0f,0.0,0.8f });
			}
		}

		Renderer2D::EndScene(command_list);
	}

	{
		PROFILE_SCOPE("Renderer ExecuteCommandList");
		command_list->ExecuteCommandList();
	}
}

void Test2D::OnImGuiRender(const light::Timestep& ts)
{
	ImGui::Begin("Profile");
	for (auto& [name,dt] : Profile::GetProfileResults())
	{
		std::string str = fmt::format("{} : {}ms", name, dt);
		ImGui::Text(str.c_str());
	}
	
	Renderer2D::Statistics stats = Renderer2D::GetStats();
	ImGui::Text("fame time:%f ms(%d fps)", ts.GetMilliseconds(), (int)(1.0f / ts.GetSeconds()));
	ImGui::Text("draw call:%d", stats.draw_calls);
	ImGui::Text("quad count:%d", stats.quad_count);
	ImGui::Text("vertex count:%d", stats.GetVertexCount());
	ImGui::Text("index count:%d", stats.GetIndexCount());

	ImGui::End();
}

void Test2D::OnEvent(light::Event& e)
{
	camera_controller_.OnEvent(e);
}
