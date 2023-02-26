#pragma once

#include <filesystem>

#include "rhi/texture.h"

#include "DirectXTex.h"
#include "stb/stb_image.h"

class TextureLoader
{
public:
	light::rhi::TextureHandle LoadTexture(std::string_view filename)
	{
		light::rhi::TextureHandle tex;

		std::filesystem::path path = filename;
		if (!exists(path))
		{
			return tex;
		}

		if(path.extension() == ".dds")
		{
#ifndef WIN32
			return tex;
#else
			//DirectX::LoadFromDDSFile(filename.data(), );
#endif
		} else
		{
			int width;
			int height;
			int channel;
			uint8_t* data = stbi_load(filename.data(), &width, &height, &channel, 0);
			if(data != nullptr)
			{
				light::rhi::TextureDesc desc;
				desc.width = width;
				desc.height = height;
				if(channel == 1)
				{
					desc.format = light::rhi::Format::R8_UINT;
				}
				else if(channel == 2)
				{
					desc.format = light::rhi::Format::RG8_UINT;
				} else if(channel == 3)
				{
					desc.format = light::rhi::Format::RGBA8_UINT;
				}
				else if (channel == 4)
				{
					desc.format = light::rhi::Format::RGBA8_UINT;
				}
			}
			
		}

		return tex;
	}
private:
};


