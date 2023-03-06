#include "engine/application.h"

using namespace light;

int main()
{
	Application* app = CreateApplication();
	app->Run();

	return 0;
}