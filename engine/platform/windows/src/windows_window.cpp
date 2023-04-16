#include "windows/windows_window.h"

#include "engine/event/windows_event.h"
#include "engine/event/key_event.h"
#include "engine/event/mouse_event.h"

#include "engine/log/log.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "windows/imgui_impl_glfw.h"

namespace light
{
	Window* CreatePlatformWindow(const WindowParams& params)
	{
		return new WindowsWindow(params);
	}

	WindowsWindow::WindowsWindow(const WindowParams& params)
		: Window(params)
		, window_(nullptr)
	{
		SetConsoleOutputCP(CP_UTF8);

		if (!glfwInit())
		{
			LOG_ENGINE_ERROR("glfw init failed!");
			return;
		}

		glfwDefaultWindowHints();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
		if (!window_)
		{
			LOG_ENGINE_ERROR("glfw windows create failed!");
			return;
		}

		glfwSetWindowUserPointer(window_, this);
		glfwShowWindow(window_);

		glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height)
		{
			auto windows_window = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

			WindowResizedEvent e(width, height);
			windows_window->OnEvent(e);
		});

		glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int width, int height)
			{
				auto windows_window = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

				WindowResizedEvent e(width, height);
				windows_window->OnEvent(e);
			});

		glfwSetWindowCloseCallback(window_, [](GLFWwindow* window)
		{
			auto windows_window = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

			WindowClosedEvent e;
			windows_window->OnEvent(e);
		});


		glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			auto windows_window = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

			if (action == GLFW_PRESS)
			{
				KeyPressedEvent e(key, 0);

				windows_window->OnEvent(e);
			}
			else if (action == GLFW_RELEASE)
			{
				KeyReleasedEvent e(key);

				windows_window->OnEvent(e);
			}
			else if (action == GLFW_REPEAT)
			{
				KeyPressedEvent e(key, 1);
				windows_window->OnEvent(e);
			}
		});

		glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods)
		{
			auto* windows_window = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

			if (action == GLFW_PRESS)
			{
				MouseButtonPressedEvent e(button, 0);

				windows_window->OnEvent(e);
			}
			else if (action == GLFW_RELEASE)
			{
				MouseButtonReleasedEvent e(button);

				windows_window->OnEvent(e);
			}
			else if (action == GLFW_REPEAT)
			{
				MouseButtonPressedEvent e(button, 1);
				windows_window->OnEvent(e);
			}
		});

		glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double xpos, double ypos)
		{
			auto* windows_window = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

			MouseMovedEvent e(xpos, ypos);
			windows_window->OnEvent(e);
		});

		glfwSetScrollCallback(window_, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			auto* windows_window = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

			MouseScrolledEvent e(xoffset, yoffset);
			windows_window->OnEvent(e);
		});

		if(ImGui::GetCurrentContext())
		{
			ImGui_ImplGlfw_InitForOther(window_, true);
			ImGui_ImplGlfw_NewFrame();
		}
	}

	WindowsWindow::~WindowsWindow()
	{
		if (window_)
		{
			glfwDestroyWindow(window_);
		}
	}

	void WindowsWindow::CloseWindow()
	{
		if (window_)
		{
			glfwDestroyWindow(window_);
		}
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();

		if (ImGui::GetCurrentContext())
		{
			ImGui_ImplGlfw_NewFrame();
		}
	}

	void* WindowsWindow::GetNativeWindow() const
	{
		return window_;
	}

	void WindowsWindow::SetVSync(bool vsync)
	{
		glfwSwapInterval(vsync ? 1 : 0);
		vsync_ = vsync;
	}

	void WindowsWindow::OnEvent(Event& event)
	{
		if (event_callback_)
		{
			event_callback_(event);
		}
	}

	void* WindowsWindow::GetHwnd()
	{
		return reinterpret_cast<void*>(glfwGetWin32Window(window_));
	}
}
