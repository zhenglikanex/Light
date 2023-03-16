#include "engine/application.h"

#include "engine/event/key_event.h"
#include "log/log.h"

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
