#include "engine/application.h"

#include "engine/log/log.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/camera.h"

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
		: running_(false)
		, timestep_(0)
		, last_frame_clock_(std::chrono::high_resolution_clock::now())
	{
		
	}

	Application::~Application()
	{
		imgui_renderer_->Shutdown();
	}

	void Application::Init()
	{
		running_ = true;

		log::Init();

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
		params.width = 800;
		params.height = 450;
		params.vsync = false;

		window_ = std::unique_ptr<Window>(CreatePlatformWindow(params));
		window_->SetEventCallback(std::bind(&Application::OnEvent, this, _1));

		device_ = rhi::DeviceHandle::Create(rhi::CreateD12Device(window_->GetHwnd()));

		swap_chain_ = device_->CreateSwapChain();

		renderer_ = std::make_unique<Renderer>(device_, swap_chain_);

		imgui_renderer_ = std::unique_ptr<rhi::ImGuiRenderer>(rhi::CreateImGuiRenderer());

		if (imgui_renderer_)
		{
			imgui_renderer_->Init(device_);
		}

		layer_stack_.PushOverlayLayer(new ImguiLayer());

		last_frame_clock_ = std::chrono::high_resolution_clock::now();
	}

	void Application::PushLayer(Layer* layer)
	{
		layer_stack_.PushLayer(layer);
	}

	void Application::PushOverlayLayer(Layer* layer)
	{
		layer_stack_.PushOverlayLayer(layer);
	}

	void Application::Run()
	{
		while (running_)
		{
			auto now = std::chrono::high_resolution_clock::now();
			auto seconds = std::chrono::duration_cast<std::chrono::duration<float>>(now - last_frame_clock_).count();
			last_frame_clock_ = now;

			timestep_ = Timestep(seconds);

			imgui_renderer_->BeginFrame();

			layer_stack_.OnUpdate(timestep_);

			imgui_renderer_->EndFrame(swap_chain_->GetRenderTarget());

			swap_chain_->Present();

			window_->OnUpdate();
		}	
	}

	Window* Application::GetWindow()
	{
		return window_.get();
	}

	void Application::OnEvent(const Event& e)
	{
		layer_stack_.OnEvent(e);

		EventDispatcher dispatcher(e);
		
		LOG_ENGINE_INFO(e);

		dispatcher.Dispatch<WindowClosedEvent>(std::bind(&Application::OnWindowClosed,this,_1));
	}

	void Application::OnWindowClosed(const WindowClosedEvent& e)
	{
		running_ = false;
	}
}