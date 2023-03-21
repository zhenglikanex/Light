#include "engine/layer/imgui_layer.h"

#include "imgui.h"

namespace light
{
	void ImguiLayer::OnAttach()
	{

	}

	void ImguiLayer::OnDetach()
	{
	}

	void ImguiLayer::OnUpdate()
	{
		static bool show_demo = true;
		ImGui::ShowDemoWindow(&show_demo);
	}

	void ImguiLayer::OnImGuiRender()
	{

	}

	void ImguiLayer::OnEvent(const Event& e)
	{

	}
}

