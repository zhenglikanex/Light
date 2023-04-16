#include "editor_layer.h"
#include "random.h"
#include "spdlog/fmt/fmt.h"

static bool block = false;

namespace light
{
	EditorLayer::EditorLayer()
		: camera_controller_(static_cast<float>(Application::Get().GetWindow()->GetWidth()) / static_cast<float>(Application::Get().GetWindow()->GetHeight()), false)
	{
		shader_library_.Load("color", rhi::ShaderType::kVertex, "assets/shaders/flat_color.hlsl");
		shader_library_.Load("color", rhi::ShaderType::kPixel, "assets/shaders/flat_color.hlsl");

		viewport_size_ = glm::vec2(0);
	}

	void EditorLayer::OnAttach()
	{
		Random::Init();
		texture_ = texture_library_.LoadTexture("assets/textures/warchessMap_4.jpg");

		RenderTargetResize({
			Application::Get().GetWindow()->GetWidth() ,
			Application::Get().GetWindow()->GetHeight()});
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(const light::Timestep& ts)
	{
		{
			PROFILE_SCOPE("CameraController OnUpdate");
			camera_controller_.OnUpdate(ts);
		}

		auto command_list = Application::Get().GetDevice()->GetCommandList(rhi::CommandListType::kDirect);
		auto render_target = Application::Get().GetRenderTarget();

		Renderer2D::ResetStats();

		{
			PROFILE_SCOPE("Renderer Draw");

			command_list->SetRenderTarget(render_target_);
			command_list->SetViewport(render_target_.GetViewport());
			command_list->SetScissorRect({ 0,0,std::numeric_limits<int32_t>::max(),std::numeric_limits<int32_t>::max() });

			constexpr float clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			command_list->ClearTexture(render_target_.GetAttachment(rhi::AttachmentPoint::kColor0).texture, clear_color);
			command_list->ClearDepthStencilTexture(render_target_.GetAttachment(rhi::AttachmentPoint::kDepthStencil).texture,
				rhi::ClearFlags::kClearFlagDepth | rhi::ClearFlags::kClearFlagStencil, 1, 0);

			Renderer2D::BeginScene(command_list, camera_controller_.GetCamera());

			static float rotation = 0;
			rotation += 10 * ts;

			Renderer2D::DrawQuad(command_list, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f), glm::vec4(1.0f));
			Renderer2D::DrawQuad(command_list, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.5f), texture_, 30);

			Renderer2D::DrawRotationQuad(command_list, glm::vec3(2.0f, 0.0f, 0.f), rotation, glm::vec2(1.0f), texture_);

			Renderer2D::DrawQuad(command_list, glm::vec2(0.0f), glm::vec2(1.0f), { 1.0,1.0,0.5,1.0 });

			Renderer2D::EndScene(command_list);
		}

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

		{
			PROFILE_SCOPE("Renderer ExecuteCommandList");
			command_list->ExecuteCommandList();
		}
	}

	void EditorLayer::OnImGuiRender(const light::Timestep& ts)
	{
		auto command_queue = Application::Get().GetDevice()->GetCommandQueue(rhi::CommandListType::kDirect);
		command_queue->WaitForFenceValue(command_queue->Signal());

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
				if (ImGui::MenuItem("Close"))
				{
					Application::Get().Close();
				}

				ImGui::Separator();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::End();

		ImGui::Begin("Profile");
		for (auto& [name, dt] : Profile::GetProfileResults())
		{
			std::string str = fmt::format("{} : {}ms", name, dt);
			ImGui::Text(str.c_str());
		}

		Renderer2D::Statistics stats = Renderer2D::GetStats();
		ImGui::Text("draw call:%d", stats.draw_calls);
		ImGui::Text("quad count:%d", stats.quad_count);
		ImGui::Text("vertex count:%d", stats.GetVertexCount());
		ImGui::Text("index count:%d", stats.GetIndexCount());
		//LOG_ENGINE_INFO("GetCursorPos X {} Y {}", ImGui::GetCursorPos().x, ImGui::GetCursorPos().y);
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
		ImGui::Begin("Viewport");
		
		block = !ImGui::IsWindowFocused() || !ImGui::IsWindowHovered();
		Application::Get().GetImguiLayer()->BlockEvent(!ImGui::IsWindowFocused() || !ImGui::IsWindowHovered());

		ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
		if(viewport_size_ != *reinterpret_cast<glm::vec2*>(&viewport_panel_size) && viewport_panel_size.x >= 1.0f && viewport_panel_size.y >= 1.0f)
		{
			viewport_size_ = *reinterpret_cast<glm::vec2*>(&viewport_panel_size);
			RenderTargetResize(viewport_size_);
			camera_controller_.OnResize(viewport_size_.x, viewport_size_.y);
		}
		ImGui::Image(rt_color_texture_->GetTextureID(), viewport_panel_size);
		ImGui::End();
		ImGui::PopStyleVar();

	}

	void EditorLayer::OnEvent(Event& e)
	{
		camera_controller_.OnEvent(e);
	}

	void EditorLayer::RenderTargetResize(const glm::vec2& size)
	{
		rhi::TextureDesc color_tex_desc;
		color_tex_desc.width = size.x;
		color_tex_desc.height = size.y;
		color_tex_desc.format = rhi::Format::RGBA8_UNORM;
		color_tex_desc.is_render_target = true;
		rhi::ClearValue color_clear_value;
		color_clear_value.color[0] = 0.0f;
		color_clear_value.color[1] = 0.0f;
		color_clear_value.color[2] = 0.0f;
		color_clear_value.color[3] = 1.0f;
		rt_color_texture_ = Application::Get().GetDevice()->CreateTexture(color_tex_desc, &color_clear_value);

		rhi::TextureDesc depth_tex_desc;
		depth_tex_desc.width = size.x;
		depth_tex_desc.height = size.y;
		depth_tex_desc.format = rhi::Format::D24S8;
		rhi::ClearValue depth_clear_value;
		depth_clear_value.depth_stencil.depth = 1.0f;
		depth_clear_value.depth_stencil.stencil = 0.0f;
		rt_depth_texture_ = Application::Get().GetDevice()->CreateTexture(depth_tex_desc, &depth_clear_value);

		render_target_.AttachAttachment(rhi::AttachmentPoint::kColor0, rt_color_texture_);
		render_target_.AttachAttachment(rhi::AttachmentPoint::kDepthStencil, rt_depth_texture_);
	}
}
