#include "engine/light.h"

#include "sandbox3d.h"

using namespace light;
using namespace rhi;

class SandboxApplication : public Application
{
public:
	SandboxApplication()
		: Application()
	{
	}

	void Init()
	{
		Application::Init();

		//PushOverlayLayer(new ExampleLayer());
		PushOverlayLayer(new Sandbox3D());
	}
private:

};

light::Application* light::CreateApplication()
{
	return new SandboxApplication();
}
