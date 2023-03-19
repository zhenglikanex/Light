#include "engine/application.h"

//#include "engine/log/log.h"
#include "engine/layer/imgui_layer.h"

#include "imgui.h"


using namespace std::placeholders;

namespace light
{
	Application* g_application = nullptr;

	Application& Application::Get()
	{
		return *g_application;
	}

	Application::Application()
		: running_(true)
	{
		//log::Init();

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

		WindowParams params;
		params.title = "Light";
		params.width = 1024;
		params.height = 768;
		params.vsync = false;

		window_ = std::unique_ptr<Window>(CreatePlatformWindow(params));
		window_->SetEventCallback(std::bind(&Application::OnEvent, this,_1));

		device_ =  rhi::DeviceHandle::Create(rhi::CreateD12Device(window_->GetHwnd()));

		swap_chain_ = device_->CreateSwapChain();

		imgui_ = std::unique_ptr<rhi::Imgui>(rhi::CreateImgui());

		if(imgui_)
		{
			imgui_->Init(device_);
		}

		layer_stack_.PushLayer(new ImguiLayer());
	}

	Application::~Application()
	{
		imgui_->Shutdown();
	}

	void Application::OnUpdate()
	{
		
	}

	void Application::Run()
	{
		while (running_)
		{
			window_->OnUpdate();

			auto command_list = device_->GetCommandList(rhi::CommandListType::kDirect);

			auto render_target = swap_chain_->GetRenderTarget();

			command_list->SetRenderTarget(render_target);
			command_list->SetViewport(render_target.GetViewport());
			command_list->SetScissorRect({ 0,0,std::numeric_limits<int32_t>::max(),std::numeric_limits<int32_t>::max() });

			constexpr float clear_color[] = { 1.0, 0.0, 0.0, 1.0 };
			command_list->ClearTexture(render_target.GetAttachment(rhi::AttachmentPoint::kColor0).texture, clear_color);

			command_list->ExecuteCommandList();

			layer_stack_.OnUpdate(render_target);

			swap_chain_->Present();
		}	
	}

	Window* Application::GetWindow()
	{
		return window_.get();
	}

	void Application::OnEvent(const Event& e)
	{
		EventDispatcher dispatcher(e);
		
		//LOG_ENGINE_INFO(e);

		dispatcher.Dispatch<WindowClosedEvent>(std::bind(&Application::OnWindowClosed,this,_1));
	}

	void Application::OnWindowClosed(const WindowClosedEvent& e)
	{
		running_ = false;
	}
}