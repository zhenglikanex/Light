#include "sandbox3d.h"
#include <algorithm>
#include "engine/utils/platform_utils.h"

using namespace light;
using namespace light::rhi;


Sandbox3D::Sandbox3D()
{
	editor_camera_.SetViewportSize(Application::Get().GetWindow()->GetWidth(), Application::Get().GetWindow()->GetHeight());	
}

void Sandbox3D::OnAttach()
{
	pbr_shader_ = shader_library_.Load("simplepbr", "assets/shaders/simplepbr.hlsl");
	hdr_shader_ = shader_library_.Load("hdr", "assets/shaders/hdr.hlsl");

	shader_library_.Load("color", "assets/shaders/texture.hlsl");

	rhi::TextureDesc color_tex_desc;
	color_tex_desc.width = Application::Get().GetWindow()->GetWidth();
	color_tex_desc.height = Application::Get().GetWindow()->GetHeight();
	color_tex_desc.format = rhi::Format::RGBA8_UNORM;
	color_tex_desc.is_render_target = true;
	rhi::ClearValue color_clear_value;
	color_clear_value.color[0] = 0.0f;
	color_clear_value.color[1] = 0.0f;
	color_clear_value.color[2] = 0.0f;
	color_clear_value.color[3] = 1.0f;
	rt_color_texture_ = Application::Get().GetDevice()->CreateTexture(color_tex_desc, &color_clear_value);

	rhi::TextureDesc depth_tex_desc;
	depth_tex_desc.width = Application::Get().GetWindow()->GetWidth();
	depth_tex_desc.height = Application::Get().GetWindow()->GetHeight();
	depth_tex_desc.format = rhi::Format::D24S8;
	rhi::ClearValue depth_clear_value;
	depth_clear_value.depth_stencil.depth = 1.0f;
	depth_clear_value.depth_stencil.stencil = 0.0f;
	rt_depth_texture_ = Application::Get().GetDevice()->CreateTexture(depth_tex_desc, &depth_clear_value);

	render_target_.AttachAttachment(rhi::AttachmentPoint::kColor0, rt_color_texture_);
	render_target_.AttachAttachment(rhi::AttachmentPoint::kDepthStencil, rt_depth_texture_);

}

void Sandbox3D::OnDetach()
{

}

void Sandbox3D::OnUpdate(const light::Timestep& ts)
{
	editor_camera_.OnUpdate(ts);

	auto command_list = Application::Get().GetDevice()->GetCommandList(CommandListType::kDirect);
	auto render_target = Application::Get().GetRenderTarget();

	{
		PROFILE_SCOPE("Renderer Prep");

		Renderer::SetupRenderTarget(command_list,render_target_);
		command_list->SetViewport(render_target_.GetViewport());
		command_list->SetScissorRect({ 0,0,std::numeric_limits<int32_t>::max(),std::numeric_limits<int32_t>::max() });

		constexpr float clear_color[] = { 0.0, 0.0, 0.0, 1.0 };
		command_list->ClearTexture(render_target_.GetAttachment(rhi::AttachmentPoint::kColor0).texture, clear_color);
		command_list->ClearDepthStencilTexture(render_target_.GetAttachment(rhi::AttachmentPoint::kDepthStencil).texture,
			rhi::ClearFlags::kClearFlagDepth | rhi::ClearFlags::kClearFlagStencil, 1, 0);
	}

	{
		PROFILE_SCOPE("Renderer Draw");
		
		Renderer::BeginScene(command_list, editor_camera_);

		Renderer::Light light;
		light.direction = light_.direction;
		light.color = light_.color;
		Renderer::SetupLight(light);
		if (mesh_)
		{
			material_->Set("cbAlbedo", pbr_material_.albedo);
			material_->Set("cbMetalness", pbr_material_.metalness);
			material_->Set("cbRoughness", pbr_material_.roughness);

			Renderer::DrawMesh(command_list, mesh_->GetMaterial(), mesh_->GetVertexBuffer(), mesh_->GetIndexBuffer(), glm::mat4(1.0f));
		}
		
		Renderer::EndScene(command_list);

		Renderer::SetupRenderTarget(command_list, render_target);
		command_list->SetViewport(render_target.GetViewport());
		command_list->SetScissorRect({ 0,0,std::numeric_limits<int32_t>::max(),std::numeric_limits<int32_t>::max() });
		constexpr float clear_color[] = { 0.1, 0.1, 0.1, 1.0 };
		command_list->ClearTexture(render_target.GetAttachment(rhi::AttachmentPoint::kColor0).texture, clear_color);
		command_list->ClearDepthStencilTexture(render_target.GetAttachment(rhi::AttachmentPoint::kDepthStencil).texture,
			rhi::ClearFlags::kClearFlagDepth | rhi::ClearFlags::kClearFlagStencil, 1, 0);

		hdr_shader_->Set("cbColor", glm::vec3(0.8));
		hdr_shader_->Set("gSourceMap", rt_color_texture_.Get());
		Renderer::DrawQuad(command_list, hdr_shader_);
	}

	{
		PROFILE_SCOPE("Renderer ExecuteCommandList");
		command_list->ExecuteCommandList();
	}
}

void Sandbox3D::OnImGuiRender(const light::Timestep& ts)
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

	ImGui::Begin("Setting");

	ImGui::Text("Light Setting");
	if (ImGui::DragFloat3("Light Direction", glm::value_ptr(light_.direction),0.1,0.1))
	{
		light_.direction = glm::normalize(light_.direction);
	}

	ImGui::ColorEdit3("Light Color", glm::value_ptr(light_.color));
	
	ImGui::Text("PBR Material");
	ImGui::ColorEdit3("Albedo", glm::value_ptr(pbr_material_.albedo));
	ImGui::DragFloat("Metalness", &pbr_material_.metalness, 0.01, 0.01, 1.0f);
	ImGui::DragFloat("Roughness", &pbr_material_.roughness, 0.01, 0.01, 1.0f);

	if (ImGui::Button("Load Mesh"))
	{
		std::string filepath = FileDialogs::OpenFile("");
		if (!filepath.empty())
		{
			CreateMesh(filepath);
		}
	}

	ImGui::End();

}

void Sandbox3D::OnEvent(light::Event& e)
{
	
}

void Sandbox3D::CreateMesh(std::string_view filename)
{
	mesh_ = MakeRef<Mesh>(filename);

	material_ = MakeRef<Material>(pbr_shader_);
	mesh_->SetMaterial(material_);
}
