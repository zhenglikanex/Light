#pragma once

#include <memory>

#include <chrono>

#include "engine/core/timestep.h"

#include "engine/event/event.h"
#include "engine/event/windows_event.h"
#include "engine/layer/layer_stack.h"

#include "engine/rhi/device.h"
#include "engine/renderer/renderer.h"
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

		virtual void Init();

		virtual void Shutdown();

		void Run();

		void PushLayer(Layer* layer);

		void PushOverlayLayer(Layer* layer);

		Window* GetWindow();

		rhi::Device* GetDevice() { return device_; }

		rhi::SwapChain* GetSwapChain() { return swap_chain_; }

		rhi::ImGuiRenderer* GetImgui() { return imgui_renderer_.get(); }

		Timestep GetTimestep() const { return timestep_; }

	private:
		void OnEvent(const Event& e);

		void OnWindowResized(const WindowResizedEvent& e);
		void OnWindowClosed(const WindowClosedEvent& e);

		bool running_;
		bool minimized_;
		std::unique_ptr<Window> window_;
		rhi::DeviceHandle device_;
		rhi::SwapChainHandle swap_chain_;
		std::unique_ptr<rhi::ImGuiRenderer> imgui_renderer_;
		LayerStack layer_stack_;
		Timestep timestep_;
		std::chrono::high_resolution_clock::time_point last_frame_clock_;
	};

	extern Application* g_application;
	extern Application* CreateApplication();
}
