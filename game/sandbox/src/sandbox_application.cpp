#include "engine/application.h"
#include "engine/layer/layer.h"
#include "engine/event/key_event.h"
#include "engine/log/log.h"
#include "engine/input/input.h"

#include <iostream>
using namespace light;

class ExampleLayer : public Layer
{
	virtual void OnUpdate() override
	{
		std::cout << "我";
		//LOG_GAME_INFO("Key 1 Is Pressed 我 ： {}",Input::IsKeyPressed(Input::Key::KEY_1));
	}

	virtual void OnImGuiRender() override
	{
		
	}

	virtual void OnEvent(const Event& e) override
	{

	}
};

class SandboxApplication : public Application
{
public:
	SandboxApplication()
		: Application()
	{
		PushOverlayLayer(new ExampleLayer());
	}
};

light::Application* light::CreateApplication()
{
	return new SandboxApplication();
}
