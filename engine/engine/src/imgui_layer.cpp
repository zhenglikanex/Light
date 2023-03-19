#include "engine/layer/imgui_layer.h"

#include "engine/application.h"
#include "engine/rhi/imgui.h"

#include "imgui.h"

namespace light
{
	void ImguiLayer::OnAttach()
	{

	}

	void ImguiLayer::OnDetach()
	{
	}

	void ImguiLayer::OnUpdate(const rhi::RenderTarget& render_target)
	{
		Application::Get().GetImgui()->BeginFrame();

		bool show_demo = true;
		ImGui::ShowDemoWindow(&show_demo);

        Application::Get().GetImgui()->OnRender(render_target);
	}

	void ImguiLayer::OnEvent(const Event& e)
	{
	}
}

