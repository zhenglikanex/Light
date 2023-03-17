#include "windows_window/windows_window.h"
#include "engine/event/windows_event.h"
#include "engine/event/key_event.h"
#include "engine/event/mouse_event.h"

#include "log/log.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

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
		if (!glfwInit())
		{
			LOG_ENGINE_ERROR("glfw init failed!")
		}

		glfwDefaultWindowHints();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
		if (!window_)
		{
			LOG_ENGINE_ERROR("glfw windows create failed!")
		}

		glfwSetWindowUserPointer(window_, this);
		glfwShowWindow(window_);

		glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height)
			{
				WindowsWindow* windows_window = reinterpret_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

				WindowResizedEvent e(width, height);
				windows_window->OnEvent(e);
			});

		glfwSetWindowCloseCallback(window_, [](GLFWwindow* window)
			{
				WindowsWindow* windows_window = reinterpret_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

				WindowClosedEvent e;
				windows_window->OnEvent(e);
			});


		glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowsWindow* windows_window = reinterpret_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

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
				WindowsWindow* windows_window = reinterpret_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

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
				WindowsWindow* windows_window = reinterpret_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

				MouseMovedEvent e(xpos, ypos);
				windows_window->OnEvent(e);
			});

		glfwSetScrollCallback(window_, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				WindowsWindow* windows_window = reinterpret_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));

				MouseScrolledEvent e(xoffset, yoffset);
				windows_window->OnEvent(e);
			});

		
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
	}

	void* WindowsWindow::GetNativeWindow() const
	{
		return nullptr;
	}

	void WindowsWindow::SetVSync(bool vsync)
	{
		glfwSwapInterval(vsync ? 1 : 0);
		vsync_ = vsync;
	}

	void WindowsWindow::OnEvent(const Event& event)
	{
		if (event_callback_)
		{
			event_callback_(event);
		}
	}

	void* WindowsWindow::GetHwnd()
	{
		glfwGetWin32Window(window_);
	}
}
