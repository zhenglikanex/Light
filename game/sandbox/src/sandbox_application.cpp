#include "engine/application.h"

using namespace light;

class SandboxApplication : public Application
{
public:
	SandboxApplication()
		: Application()
	{
		
	}
};

light::Application* light::CreateApplication()
{
	return new SandboxApplication();
}
