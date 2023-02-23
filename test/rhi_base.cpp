#include "rhi_base.h"

#ifdef _WIN32
#include "Windows.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#undef max
#undef min

using CreateDeviceFunc = light::rhi::Device* (*)(size_t);

namespace light
{
	void FramebufferSizecallback(GLFWwindow* window, int width, int height)
	{
		Game* game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
		if(game)
		{
			game->OnResize(width, height);
		}
	}

	void KeyCallback()
	{
		
	}

	Game::Game(const GameParams& desc)
		: params_(desc)
		, window_(nullptr)
		, last_frame_time_(0)
	{

	}

	bool Game::Initialize()
	{
		if(!InitWindow())
		{
			return false;
		}

		if(!InitDeviceAndSwapChain())
		{
			return false;
		}

		return OnInit();
	}

	void Game::Shutdown()
	{
		device_->Flush();

		if(window_)
		{
			glfwDestroyWindow(window_);
			window_ = nullptr;
		}

		glfwTerminate();
	}

	void Game::Run()
	{
		last_frame_time_ = glfwGetTime();

		while(!glfwWindowShouldClose(window_))
		{
			glfwPollEvents();

			double cur_time = glfwGetTime();
			double dt = cur_time - last_frame_time_;

			//AutoTimer update("Update");
			OnUpdate(dt);
			OnRender(dt);
		}

	}

	void Game::OnResize(uint32_t width, uint32_t height)
	{
		swap_chain_->Resize(width, height);
	}

	bool Game::InitWindow()
	{
		if(!glfwInit())
		{
			return false;
		}

		glfwDefaultWindowHints();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window_ = glfwCreateWindow(params_.width, params_.height, "Light Game", nullptr, nullptr);
		if(!window_)
		{
			return false;
		}

		glfwSetWindowUserPointer(window_, this);
		glfwSetFramebufferSizeCallback(window_, FramebufferSizecallback);

		glfwShowWindow(window_);

		return true;
	}

	bool Game::InitDeviceAndSwapChain()
	{
#ifdef _WIN32
		
		switch (params_.api) 
		{
		case rhi::GraphicsApi::kNone:
			//device_ = nullptr;
			break;
		case rhi::GraphicsApi::kD3D12:
		{
			rhi_module_ = LoadLibrary(L"D12RHI.dll");
			auto func = reinterpret_cast<CreateDeviceFunc>(GetProcAddress(rhi_module_, "CreateDevice"));
			if (func)
			{
				device_ = func(reinterpret_cast<size_t>(glfwGetWin32Window(window_)));
			}
		}
			break;
		case rhi::GraphicsApi::kVulkan:
			//device_ = nullptr;
			break;
		default:;
			//device_ = nullptr;
		}
#endif

		if(!device_)
		{
			return false;
		}

		swap_chain_ = device_->CreateSwapChain();

		if(!swap_chain_)
		{
			return false;
		}

		return true;
	}
}
