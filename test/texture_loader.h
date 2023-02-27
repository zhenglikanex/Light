#pragma once

#include <filesystem>
#include <fstream>

#include "rhi/texture.h"
#include "rhi/device.h"

#include "DirectXTex.h"
#include "stb/stb_image.h"

class TextureLoader
{
public:
	light::rhi::TextureHandle LoadTexture(light::rhi::Device* device,std::string_view filename,bool force_srgb = false)
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
			
			std::ifstream fin(filename.data());
			if (fin.is_open())
			{
				fin.seekg(0, std::ios_base::end);
				uint64_t file_size = fin.tellg();
				fin.seekg(0, std::ios_base::beg);

				std::vector<char> data(file_size);
				fin.read(data.data(), file_size);

				light::rhi::TextureDesc desc;

				uint8_t* buf = (uint8_t*)data.data();
				size_t len = data.size();
				if (stbi_is_16_bit_from_memory(buf, len)) 
				{
					int width;
					int height;
					int channel;
					
					auto image_data = stbi_load_16_from_memory(buf, len, &width, &height, &channel,0);

					// ǿ�ƶ�ȡRGBA��ʽ,����ͼ��API��֧��RGB����,��(DirteX,Vualkan)
					if (channel == 3)
					{
						channel == 4;
					}

					desc.width = width;
					desc.height = height;

					if (channel == 1)
					{
						desc.format = light::rhi::Format::R16_UNORM;
					}
					else if (channel == 2)
					{
						desc.format = light::rhi::Format::RG16_UNORM;
					}
					else if (channel == 4)
					{
						desc.format = light::rhi::Format::RGBA16_UNORM;
					}
				}
				else if(stbi_is_hdr_from_memory(buf,len))
				{
					int width;
					int height;
					int channel;
					
					auto image_data = stbi_loadf_from_memory(buf, len, &width, &height, &channel,0);

					desc.width = width;
					desc.height = height;

					if (channel == 1)
					{
						desc.format = light::rhi::Format::R32_FLOAT;
					}
					else if (channel == 2)
					{
						desc.format = light::rhi::Format::RG32_FLOAT;
					}
					else if (channel == 3)
					{
						desc.format = light::rhi::Format::RGB32_FLOAT;
					}
					else if (channel == 4)
					{
						desc.format = light::rhi::Format::RGBA32_FLOAT;
					}
				}
				else
				{
					int width;
					int height;
					int channel;
					// ǿ�ƶ�ȡRGBA��ʽ,����ͼ��API��֧��RGB����,��(DirteX,Vualkan)
					auto image_data = stbi_load_from_memory(buf, len, &width, &height, &channel, 0);

					// ǿ�ƶ�ȡRGBA��ʽ,����ͼ��API��֧��RGB����,��(DirteX,Vualkan)
					if (channel == 3)
					{
						channel == 4;
					}

					desc.width = width;
					desc.height = height;

					if (channel == 1)
					{
						desc.format = light::rhi::Format::R8_UNORM;
					}
					else if (channel == 2)
					{
						desc.format = light::rhi::Format::RG8_UNORM;
					}
					else if (channel == 4)
					{
						desc.format = force_srgb ? light::rhi::Format::SRGBA8_UNORM : light::rhi::Format::RGBA8_UNORM;
					}
				}

				tex = device->CreateTexture(desc);
				
			}

			
		}

		return tex;
	}
private:
};

