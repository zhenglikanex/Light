#include "sandbox2d.h"
#include "random.h"
#include "spdlog/fmt/fmt.h"



using namespace light;
using namespace light::rhi;

Sandbox2D::Sandbox2D()
	: camera_controller_(static_cast<float>(Application::Get().GetWindow()->GetWidth()) / static_cast<float>(Application::Get().GetWindow()->GetHeight()), false)
{
	shader_library_.Load("color", ShaderType::kVertex, "assets/shaders/flat_color.hlsl");
	shader_library_.Load("color", ShaderType::kPixel, "assets/shaders/flat_color.hlsl");

}

void Sandbox2D::OnAttach()
{
	Random::Init();
	texture_ = texture_library_.LoadTexture("assets/textures/warchessMap_4.jpg");
	
	scene_ = MakeRef<Scene>();

	quad_entity_ = scene_->CreateEntity("quad");
	quad_entity_.AddComponent<SpriteRendererComponent>(glm::vec4(1, 1, 1, 1));
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

		scene_->OnUpdate(ts, command_list);

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

	if (quad_entity_)
	{
		auto& sprite = quad_entity_.GetComponent<SpriteRendererComponent>();
		ImGui::ColorEdit4("quad_color",glm::value_ptr(sprite.color));
	}

	ImGui::End();

}

void Sandbox2D::OnEvent(light::Event& e)
{
	camera_controller_.OnEvent(e);
}
