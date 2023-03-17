#include "engine/application.h"

#include "engine/log/log.h"

using namespace std::placeholders;

using CreateDeviceFunc = light::rhi::Device* (*)(void*);
using CreateImguiFunc = light::rhi::Imgui* (*)();

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
		log::Init();

		WindowParams params;
		params.title = "Light";
		params.width = 1024;
		params.height = 768;
		params.vsync = false;

		window_ = std::unique_ptr<Window>(CreatePlatformWindow(params));
		window_->SetEventCallback(std::bind(&Application::OnEvent, this,_1));

		rhi_module_ = LoadLibrary(L"D12RHI.dll");
		auto func = reinterpret_cast<CreateDeviceFunc>(GetProcAddress(rhi_module_, "CreateDevice"));
		if (func)
		{
			device_ = func(window_->GetHwnd());
		}

		auto create_imgui = reinterpret_cast<CreateImguiFunc>(GetProcAddress(rhi_module_, "CreateImgui"));
		if (create_imgui)
		{
			imgui_ = create_imgui();
		}

	}

	Application::~Application()
	{
		
	}

	void Application::OnUpdate()
	{
		
	}

	void Application::Run()
	{
		while (running_)
		{
			layer_stack_.OnUpdate();

			window_->OnUpdate();
		}	
	}

	Window* Application::GetWindow()
	{
		return window_.get();
	}

	void Application::OnEvent(const Event& e)
	{
		EventDispatcher dispatcher(e);
		
		LOG_ENGINE_INFO(e);

		dispatcher.Dispatch<WindowClosedEvent>(std::bind(&Application::OnWindowClosed,this,_1));
	}

	void Application::OnWindowClosed(const WindowClosedEvent& e)
	{
		running_ = false;
	}
}