#pragma once

#include <memory>

#include "engine/event/event.h"
#include "engine/event/windows_event.h"
#include "engine/layer/layer_stack.h"
#include "engine/rhi/device.h"
#include "engine/rhi/imgui_renderer.h"

#include "engine/platform/window/window.h"

namespace light
{
	class Window;

	class Application
	{
	public:
		static Application& Get();

		Application();
		virtual ~Application();

		virtual void OnUpdate();

		void Run();

		Window* GetWindow();

		rhi::Device* GetDevice() { return device_; }

		rhi::ImGuiRenderer* GetImgui() { return imgui_renderer_.get(); }
	private:
		void OnEvent(const Event& e);

		void OnWindowClosed(const WindowClosedEvent& e);

		bool running_;
		std::unique_ptr<Window> window_;
		rhi::DeviceHandle device_;
		rhi::SwapChainHandle swap_chain_;
		std::unique_ptr<rhi::ImGuiRenderer> imgui_renderer_;
		LayerStack layer_stack_;
	};

	extern Application* g_application;
	extern Application* CreateApplication();
}
