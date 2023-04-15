#include "engine/core/application.h"

#include "engine/log/log.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/renderer2d.h"
#include "engine/renderer/camera.h"
#include "engine/layer/imgui_layer.h"
#include "engine/profile/profile.h"

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
		, minimized_(false)
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

		Log::Init();

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

		WindowParams window_params;
		window_params.title = "Light";
		window_params.width = 800;
		window_params.height = 450;
		window_params.vsync = true;

		window_ = std::unique_ptr<Window>(CreatePlatformWindow(window_params));
		window_->SetEventCallback(std::bind(&Application::OnEvent, this, _1));

		device_ = rhi::DeviceHandle::Create(rhi::CreateD12Device(window_->GetHwnd()));

		swap_chain_ = device_->CreateSwapChain();
		swap_chain_->SetVSync(window_params.vsync);

		rhi::TextureDesc depth_desc;
		depth_desc.format = rhi::Format::D24S8;
		depth_desc.width = window_params.width;
		depth_desc.height = window_params.height;
		depth_texture_ = device_->CreateTexture(depth_desc);

		imgui_renderer_ = std::unique_ptr<rhi::ImGuiRenderer>(rhi::CreateImGuiRenderer());

		if (imgui_renderer_)
		{
			imgui_renderer_->Init(device_);
		}

		Renderer2D::Init();

		layer_stack_.PushOverlayLayer(new ImguiLayer());

		last_frame_clock_ = std::chrono::high_resolution_clock::now();
	}

	void Application::Shutdown()
	{
		Renderer2D::Shutdown();
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

			layer_stack_.OnUpdate(timestep_, minimized_);

			imgui_renderer_->EndFrame(swap_chain_->GetRenderTarget());

			swap_chain_->Present();

			window_->OnUpdate();

			Profile::ClearProfileResult();
		}	
	}

	void Application::Close()
	{
		running_ = false;
	}

	Window* Application::GetWindow()
	{
		return window_.get();
	}

	rhi::RenderTarget Application::GetRenderTarget()
	{
		auto render_target = swap_chain_->GetRenderTarget();
		render_target.AttachAttachment(rhi::AttachmentPoint::kDepthStencil, depth_texture_);
		return render_target;
	}

	void Application::OnEvent(const Event& e)
	{
		layer_stack_.OnEvent(e);

		EventDispatcher dispatcher(e);
		
		LOG_ENGINE_INFO(e);

		dispatcher.Dispatch<WindowClosedEvent>(std::bind(&Application::OnWindowClosed,this,_1));
		dispatcher.Dispatch<WindowResizedEvent>(std::bind(&Application::OnWindowResized, this, _1));
	}

	void Application::OnWindowResized(const WindowResizedEvent& e)
	{
		minimized_ = e.width == 0 || e.height == 0;
		if (minimized_)
		{
			return;
		}

		imgui_renderer_->Flush();
		swap_chain_->Resize(e.width, e.height);

		rhi::TextureDesc depth_desc;
		depth_desc.format = rhi::Format::D24S8;
		depth_desc.width = e.width;
		depth_desc.height = e.height;
		depth_texture_ = device_->CreateTexture(depth_desc);
	}

	void Application::OnWindowClosed(const WindowClosedEvent& e)
	{
		running_ = false;
	}
}