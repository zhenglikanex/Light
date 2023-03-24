#include "engine/application.h"

using namespace light;

int main()
{
	light::g_application = light::CreateApplication();
	light::g_application->Init();
	light::g_application->Run();
	delete g_application;
	return 0;
}