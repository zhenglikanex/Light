#include "editor_layer.h"
#include "random.h"
#include "spdlog/fmt/fmt.h"


static bool block = false;

namespace light
{
	EditorLayer::EditorLayer()
	{
		viewport_size_ = glm::vec2(0);
	}

	void EditorLayer::OnAttach()
	{
		Random::Init();
		texture_ = texture_library_.LoadTexture("assets/textures/warchessMap_4.jpg");

		viewport_size_ = { Application::Get().GetWindow()->GetWidth(), Application::Get().GetWindow()->GetHeight() };
		RenderTargetResize(viewport_size_);

		active_secne_ = MakeRef<Scene>();

		quad_entity_ = active_secne_->CreateEntity("quad");
		quad_entity_.AddComponent<SpriteRendererComponent>(glm::vec4(1, 1, 1, 1));
		
		//create camera
		camera_entity_ = active_secne_->CreateEntity("camera");

		// 计算横纵比
		float aspect = Application::Get().GetWindow()->GetWidth() / Application::Get().GetWindow()->GetHeight();
		auto& camera_component = camera_entity_.AddComponent<CameraComponent>();

		class CameraController : public Script
		{
		public:
			void OnUpdate(Timestep ts) override
			{
				auto& transform = entity.GetComponent<TransformComponent>();
				float speed = 5.0f;
				if (Input::IsKeyPressed(Input::Key::KEY_W))
					transform.transform[3][1] += speed * ts;
				if (Input::IsKeyPressed(Input::Key::KEY_S))
					transform.transform[3][1] -= speed * ts;
				if (Input::IsKeyPressed(Input::Key::KEY_A))
					transform.transform[3][0] -= speed * ts;
				if (Input::IsKeyPressed(Input::Key::KEY_D))
					transform.transform[3][0] += speed * ts;
			}
		};

		camera_entity_.AddComponent<NativeScriptComponent>().Bind<CameraController>();
	}

	void EditorLayer::OnDetach()
	{
		
	}

	void EditorLayer::OnUpdate(const light::Timestep& ts)
	{
		if (viewport_size_.x != render_target_.GetWidth() || viewport_size_.y != render_target_.GetHeight())
		{
			RenderTargetResize(viewport_size_);
			
			active_secne_->SetViewportSize(render_target_.GetWidth(),render_target_.GetHeight());
		}

		auto command_list = Application::Get().GetDevice()->GetCommandList(rhi::CommandListType::kDirect);
		auto render_target = Application::Get().GetRenderTarget();

		Renderer2D::ResetStats();

		{
			PROFILE_SCOPE("Renderer Draw");

			command_list->SetRenderTarget(render_target_);
			command_list->SetViewport(render_target_.GetViewport());
			command_list->SetScissorRect({ 0,0,std::numeric_limits<int32_t>::max(),std::numeric_limits<int32_t>::max() });

			constexpr float clear_color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
			command_list->ClearTexture(render_target_.GetAttachment(rhi::AttachmentPoint::kColor0).texture, clear_color);
			command_list->ClearDepthStencilTexture(render_target_.GetAttachment(rhi::AttachmentPoint::kDepthStencil).texture,
				rhi::ClearFlags::kClearFlagDepth | rhi::ClearFlags::kClearFlagStencil, 1, 0);

			active_secne_->OnUpdate(ts, command_list);
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
		auto commnad_list = Application::Get().GetDevice()->GetCommandList(rhi::CommandListType::kDirect);
		commnad_list->TransitionBarrier(rt_color_texture_, rhi::ResourceStates::kPixelShaderResource);
		commnad_list->ExecuteCommandList();

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

		ImGui::Begin("Settings");
		
		auto& sprite = quad_entity_.GetComponent<SpriteRendererComponent>();
		ImGui::ColorEdit4("Quad Color", glm::value_ptr(sprite.color));

		auto& camera_transform = camera_entity_.GetComponent<TransformComponent>().transform;
		ImGui::DragFloat3("Camera Position", glm::value_ptr(camera_transform[3]), 0.1f);

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
		}


		ImGui::Image(rt_color_texture_->GetTextureID(), viewport_panel_size);
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		
	}

	void EditorLayer::RenderTargetResize(const glm::vec2& size)
	{
		Application::Get().GetDevice()->Flush();

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
