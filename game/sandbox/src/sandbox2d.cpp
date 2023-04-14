#include "sandbox2d.h"
#include "random.h"
#include "spdlog/fmt/fmt.h"

using namespace light;
using namespace light::rhi;

Sandbox2D::Sandbox2D()
	:camera_controller_(800.0f / 450.0f, false)
{	
	shader_library_.Load("color", ShaderType::kVertex, "assets/shaders/flat_color.hlsl");
	shader_library_.Load("color", ShaderType::kPixel, "assets/shaders/flat_color.hlsl");
}

void Sandbox2D::OnAttach()
{
	Random::Init();
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

		/*Renderer2D::DrawQuad(command_list, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f), glm::vec4(1.0f));
		Renderer2D::DrawQuad(command_list, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.5f), texture_, 30);

		Renderer2D::DrawRotationQuad(command_list, glm::vec3(2.0f, 0.0f, 0.f), rotation,glm::vec2(1.0f), texture_);

		Renderer2D::DrawQuad(command_list, glm::vec2(0.0f), glm::vec2(1.0f), { 1.0,1.0,0.5,1.0 });*/


		if (Input::IsMouseButtonPressed(Input::MouseButton::BUTTON_LEFT))
		{
			ParticleProps props;
			props.position = { ((Input::GetMouseX() - 400.0f) / 400.0f + camera_controller_.GetCamera().GetPosition().x) * 800.f / 450.f, -(Input::GetMouseY() - 225.0f) / 225.0f + camera_controller_.GetCamera().GetPosition().y };
			props.size_begin = 0.1f;
			props.size_end = 0.0f;
			props.size_variation = 0.2f;
			props.color_begin = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			props.color_end = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
			props.life_time = 2.0f;
			props.velocity = glm::vec3(0.3f, 0.2f, 0.0f);
			props.velocity_variation = glm::vec2(0.5,0.3);

			for (uint32_t i = 0; i < 10; ++i)
			{
				props.color_begin = glm::vec4(Random::Float(), Random::Float(), Random::Float(), 1.0f);
				props.color_end = glm::vec4(Random::Float(), Random::Float(), Random::Float(), 0.0f);
				particle_system_.Emit(props);
			}
		}

		particle_system_.OnUpdate(ts);
		particle_system_.OnRender(command_list);

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
	for (auto& [name,dt] : Profile::GetProfileResults())
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
}

void Sandbox2D::OnEvent(const light::Event& e)
{
	camera_controller_.OnEvent(e);
}
