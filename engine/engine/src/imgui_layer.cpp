#include "engine/layer/imgui_layer.h"

#include "engine/application.h"
#include "engine/rhi/imgui.h"

#include "imgui.h"


namespace light
{
	void ImguiLayer::OnAttach()
	{
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        Application::Get().GetImgui()->Init(Application::Get().GetDevice());
	}

	void ImguiLayer::OnDeattach()
	{
        Application::Get().GetImgui()->Shutdown();
        ImGui::DestroyContext();
	}

	void ImguiLayer::OnUpdate()
	{
        
        Application::Get().GetImgui()->OnRender();
	}

	void ImguiLayer::OnEvent(const Event& e)
	{
	}
}

