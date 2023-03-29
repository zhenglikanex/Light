#include "engine/core/application.h"

using namespace light;

int main()
{
	light::g_application = light::CreateApplication();
	light::g_application->Init();
	light::g_application->Run();
	light::g_application->Shutdown();
	delete g_application;
	return 0;
}