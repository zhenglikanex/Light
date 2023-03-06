#include "engine/application.h"

namespace light
{
	static Application* s_application = nullptr;

	Application& Application::Get()
	{
		return *s_application;
	}

	Application::Application()
		: running_(true)
	{

	}

	void Application::OnUpdate()
	{
		
	}

	void Application::Run()
	{
		while (running_)
		{
			OnUpdate();
		}
	}
}