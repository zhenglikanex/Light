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
	shader_ = shader_library_.Load("simplepbr", "assets/shaders/simplepbr.hlsl");
	shader_library_.Load("color", "assets/shaders/texture.hlsl");
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

		command_list->SetRenderTarget(render_target);
		command_list->SetViewport(render_target.GetViewport());
		command_list->SetScissorRect({ 0,0,std::numeric_limits<int32_t>::max(),std::numeric_limits<int32_t>::max() });

		constexpr float clear_color[] = { 0.1, 0.1, 0.1, 1.0 };
		command_list->ClearTexture(render_target.GetAttachment(rhi::AttachmentPoint::kColor0).texture, clear_color);
		command_list->ClearDepthStencilTexture(render_target.GetAttachment(rhi::AttachmentPoint::kDepthStencil).texture,
			rhi::ClearFlags::kClearFlagDepth | rhi::ClearFlags::kClearFlagStencil, 1, 0);
	}

	{
		PROFILE_SCOPE("Renderer Draw");
		Renderer::BeginScene(command_list, render_target, editor_camera_);

		Renderer::Light light;
		light.direction = light_.direction;
		light.color = light_.color;
		Renderer::SetupLight(light);
		if (mesh_)
		{
			std::vector<uint8_t> buffer(sizeof(PbrMaterial));
			memcpy(buffer.data(), &pbr_material_, buffer.size());

			material_->SetParamsBuffer(std::move(buffer));

			Renderer::DrawMesh(command_list, mesh_->GetMaterial(), mesh_->GetVertexBuffer(), mesh_->GetIndexBuffer(), glm::mat4(1.0f));
		}

		Renderer::EndScene(command_list);
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

	material_ = MakeRef<Material>(shader_);
	mesh_->SetMaterial(material_);
}
