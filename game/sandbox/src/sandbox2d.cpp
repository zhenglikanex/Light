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
	static bool show_dockspace = true;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &show_dockspace, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
			if(ImGui::MenuItem("Close"))
			{
				
			}

            ImGui::Separator();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();

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

	static bool show = true;
	ImGui::ShowDemoWindow(&show);
}

void Sandbox2D::OnEvent(const light::Event& e)
{
	camera_controller_.OnEvent(e);
}
