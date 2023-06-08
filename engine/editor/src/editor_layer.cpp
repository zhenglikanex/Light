#include "editor_layer.h"
#include "random.h"

#include "engine/math/math.h"
#include "engine/utils/platform_utils.h"

#include "imguizmo/ImGuizmo.h"

namespace light::editor
{
	EditorLayer::EditorLayer()
	{
		viewport_size_ = glm::vec2(0);
		guizmo_type_ = ImGuizmo::TRANSLATE;
	}

	void EditorLayer::OnAttach()
	{
		Random::Init();
		texture_ = texture_library_.LoadTexture("assets/textures/warchessMap_4.jpg");

		viewport_size_ = { Application::Get().GetWindow()->GetWidth(), Application::Get().GetWindow()->GetHeight() };
		RenderTargetResize(viewport_size_);

		active_secne_ = MakeRef<Scene>();
		scene_hierarchy_panel_.SetScene(active_secne_);

		class CameraController : public Script
		{
		public:
			void OnUpdate(Timestep ts) override
			{
				auto& transform = entity.GetComponent<TransformComponent>();
				float speed = 5.0f;
				if (Input::IsKeyPressed(Input::Key::KEY_W))
					transform.position.y += speed * ts;
				if (Input::IsKeyPressed(Input::Key::KEY_S))
					transform.position.y -= speed * ts;
				if (Input::IsKeyPressed(Input::Key::KEY_A))
					transform.position.x -= speed * ts;
				if (Input::IsKeyPressed(Input::Key::KEY_D))
					transform.position.x += speed * ts;
			}
		};
	}

	void EditorLayer::OnDetach()
	{
		
	}

	void EditorLayer::OnUpdate(const light::Timestep& ts)
	{
		editor_camera_.OnUpdate(ts);

		if (viewport_size_.x > 0.f && viewport_size_.y > 0.0f && (viewport_size_.x != render_target_.GetWidth() || viewport_size_.y != render_target_.GetHeight()))
		{
			RenderTargetResize(viewport_size_);

			active_secne_->SetViewportSize(render_target_.GetWidth(), render_target_.GetHeight());
			editor_camera_.SetViewportSize(render_target_.GetWidth(), render_target_.GetHeight());
		}

		auto command_list = Application::Get().GetDevice()->GetCommandList(rhi::CommandListType::kDirect);
		auto render_target = Application::Get().GetRenderTarget();

		Renderer2D::ResetStats();

		{
			PROFILE_SCOPE("Renderer Draw");

			active_secne_->OnUpdateEditor(ts, command_list, render_target_, editor_camera_);
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
				if (ImGui::MenuItem("New", "Ctrl+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Open...","Ctrl+O"))
				{
					std::string filepath = FileDialogs::OpenFile("Light Scene(*.scene)\0*.scene\0");
					if (!filepath.empty())
					{
						Ref<Scene> scene = MakeRef<Scene>();
						SceneSerializer serializer(scene);
						serializer.DeserializeText(filepath);

						active_secne_ = scene;
						active_secne_->SetViewportSize(render_target_.GetWidth(), render_target_.GetHeight());
						scene_hierarchy_panel_.SetScene(active_secne_);
					}
				}

				if (ImGui::MenuItem("Save As...","Ctrl+Shift+S"))
				{
					std::string filepath = FileDialogs::SaveFile("test.scene","Light Scene (*.scene)\0*.scene\0");
					if (!filepath.empty())
					{
						SceneSerializer serializer(active_secne_);
						serializer.SerializeText(filepath);
					}
				}

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

		ImGui::Begin("SceneRenderSettings");

		

		ImGui::End();

		ImGui::Begin("Profile");
		for (auto& [name, dt] : Profile::GetProfileResults())
		{
			std::string str = std::format("{} : {}ms", name, dt);
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
		
		Application::Get().GetImguiLayer()->BlockEvent(!ImGui::IsWindowFocused() || !ImGui::IsWindowHovered());

		ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
		if(viewport_size_ != *reinterpret_cast<glm::vec2*>(&viewport_panel_size) && viewport_panel_size.x >= 1.0f && viewport_panel_size.y >= 1.0f)
		{
			viewport_size_ = *reinterpret_cast<glm::vec2*>(&viewport_panel_size);
		}

		ImGui::Image(rt_color_texture_->GetTextureID(), viewport_panel_size);

		Entity selected_entity = scene_hierarchy_panel_.GetSelectedEntity();
		if (selected_entity)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

			glm::mat4 projection = editor_camera_.GetProjectionMatrx();
			glm::mat4 view_matrix = editor_camera_.GetViewMatrix();

			auto& tc = selected_entity.GetComponent<TransformComponent>();
			glm::mat4 model_matrix = tc.GetTransform();

			ImGuizmo::Manipulate(glm::value_ptr(view_matrix), glm::value_ptr(projection), (ImGuizmo::OPERATION)guizmo_type_, ImGuizmo::LOCAL, glm::value_ptr(model_matrix));

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 rotation;

				math::Decompose(model_matrix, tc.position, rotation, tc.scale);
				tc.rotation = glm::degrees(rotation);
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		scene_hierarchy_panel_.OnImguiRender();
		property_panel_.OnImguiRender();

		if (selected_entity)
		{
			if (selected_entity.HasComponent<MeshComponent>())
			{
				auto& mesh_comp = selected_entity.GetComponent<MeshComponent>();
				if (mesh_comp.mesh)
				{
					Material* material = mesh_comp.mesh->GetMaterial(0);
					if (material)
					{
						material_panel_.SelectMaterial(material);
					}
				}
			}
			
			material_panel_.OnImguiRender();
		}
	}

	void EditorLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_1(&EditorLayer::OnKeyPressedEvent, this));

		property_panel_.OnEvent(e);
	}

	void EditorLayer::OnKeyPressedEvent(const KeyPressedEvent& e)
	{
		if (e.keycode == (int)Input::Key::KEY_W)
		{
			guizmo_type_ = ImGuizmo::TRANSLATE;
		}
		else if (e.keycode == (int)Input::Key::KEY_E)
		{
			guizmo_type_ = ImGuizmo::SCALE;
		}
		else if (e.keycode == (int) Input::Key::KEY_R)
		{
			guizmo_type_ = ImGuizmo::ROTATE;
		}

		if (e.keycode == (int)Input::Key::KEY_F)
		{
			Entity e = scene_hierarchy_panel_.GetSelectedEntity();
			if (e)
			{
				glm::vec3 position = e.GetComponent<TransformComponent>().position;

				editor_camera_.SetPosition(position - glm::vec3(0, 0, 10));
				editor_camera_.SetRotation(glm::vec3(0));
			}
		}
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

		/*rhi::TextureDesc color2_tex_desc;
		color2_tex_desc.width = size.x;
		color2_tex_desc.height = size.y;
		color2_tex_desc.format = rhi::Format::RGBA8_UNORM;
		color2_tex_desc.is_render_target = true;
		rt_color2_texture_ = Application::Get().GetDevice()->CreateTexture(color2_tex_desc);*/


		rhi::TextureDesc depth_tex_desc;
		depth_tex_desc.width = size.x;
		depth_tex_desc.height = size.y;
		depth_tex_desc.format = rhi::Format::D24S8;
		depth_tex_desc.debug_name = "editor_layer_depth_tex";
		rhi::ClearValue depth_clear_value;
		depth_clear_value.depth_stencil.depth = 1.0f;
		depth_clear_value.depth_stencil.stencil = 0.0f;
		rt_depth_texture_ = Application::Get().GetDevice()->CreateTexture(depth_tex_desc, &depth_clear_value);

		render_target_.AttachAttachment(rhi::AttachmentPoint::kColor0, rt_color_texture_);
		//render_target_.AttachAttachment(rhi::AttachmentPoint::kColor1, rt_color2_texture_);
		render_target_.AttachAttachment(rhi::AttachmentPoint::kDepthStencil, rt_depth_texture_);
	}

	void EditorLayer::NewScene()
	{
		active_secne_ = MakeRef<Scene>();
		active_secne_->SetViewportSize(render_target_.GetWidth(), render_target_.GetHeight());
		scene_hierarchy_panel_.SetScene(active_secne_);

		Entity cube = active_secne_->CreateEntity("Cube");
		cube.AddComponent<MeshComponent>(MakeRef<Mesh>("assets/models/mayaCube.obj"));

		Entity plane = active_secne_->CreateEntity("Plane");
		plane.AddComponent<MeshComponent>(MakeRef<Mesh>("assets/models/Plane.fbx"));
		plane.GetComponent<TransformComponent>().position.y = -1.0f;
		plane.GetComponent<TransformComponent>().scale.x = 10;
		plane.GetComponent<TransformComponent>().scale.z = 10;


		Entity light = active_secne_->CreateEntity("Light");
		light.AddComponent<LightComponent>(glm::vec3(1.0f));

		auto& transform_comp = light.GetComponent<TransformComponent>();
		transform_comp.position.z = 0;
		transform_comp.rotation.x = 24;
	}
}
