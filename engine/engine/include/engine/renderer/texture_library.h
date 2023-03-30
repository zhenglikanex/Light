#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>

#include "engine/core/application.h"
#include "engine/rhi/device.h"

namespace light
{
	class TextureLibrary
	{
	public:
		rhi::TextureHandle LoadTexture(std::string_view filename, bool force_srgb = false);
	private:
	};
}



