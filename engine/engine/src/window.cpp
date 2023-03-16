#include "engine/platform/window/window.h"

namespace light
{
	Window::Window(const WindowParams& params)
		: title_(params.title)
		, width_(params.width)
		, height_(params.height)
		, vsync_(params.vsync)
	{

	}
}