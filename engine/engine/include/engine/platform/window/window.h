#pragma once

#include <memory>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

#include "engine/event/event.h"

namespace light
{
	struct WindowParams
	{
		std::string_view title;
		uint32_t width = 1280;
		uint32_t height = 720;
		bool vsync = false;
	};

	class Window
	{
	public:
		Window(const WindowParams& params);

		virtual ~Window() = default;

		virtual void CloseWindow() = 0;

		virtual void OnUpdate() = 0;

		virtual void* GetHwnd() = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual void SetVSync(bool vsync) = 0;

		void SetEventCallback(const std::function<void(Event&)>& callback) { event_callback_ = callback; }

		uint32_t GetWidth() const { return width_; }
		uint32_t GetHeight() const { return height_; }

		bool IsVSync() const { return vsync_; }
	protected:
		std::string title_;
		uint32_t width_;
		uint32_t height_;
		bool vsync_;
		std::function<void(Event&)> event_callback_;
	};

	extern Window* CreatePlatformWindow(const WindowParams& params);
}