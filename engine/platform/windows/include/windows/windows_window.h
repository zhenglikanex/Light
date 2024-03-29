#pragma once

#include "engine/platform/window/window.h"
#include "engine/event/event.h"

#include "GLFW/glfw3.h"

namespace light
{
	class WindowsWindow : public Window
	{
	public:
		explicit WindowsWindow(const WindowParams& params);

		~WindowsWindow() override;

		void CloseWindow() override;

		void OnUpdate() override;

		void* GetNativeWindow() const override;

		void* GetHwnd() override;

		void SetVSync(bool vsync);

		void OnEvent(Event& event);
	private:
		GLFWwindow* window_;

		// 通过 Window 继承
		
	};
}
