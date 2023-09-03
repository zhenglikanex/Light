#include "engine/renderer/render_command_stream.h"
#include <iostream>
int main()
{
	light::RenderCommandStream stream;
	int count = 10;
	stream.Submit([count]()
		{
			std::cout << count << std::endl;
		});
	stream.Execute();


	return 0;
}