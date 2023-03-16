#include "engine/application.h"

int main()
{
	light::g_application = light::CreateApplication();
	light::g_application->Run();

	return 0;
}