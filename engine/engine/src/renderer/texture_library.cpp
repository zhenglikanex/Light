#include "engine/renderer/texture_library.h"

#include "stb_image.h"

namespace light
{
	rhi::TextureHandle TextureLibrary::LoadTexture(std::string_view filename, bool force_srgb /* = false */)
	{
		rhi::TextureHandle tex;

		std::filesystem::path path = filename;
		if (!exists(path))
		{
			return tex;
		}

		if (path.extension() == ".dds")
		{
#ifndef WIN32
			return tex;
#else
			//DirectX::LoadFromDDSFile(filename.data(), );
#endif
		}
		else
		{
			std::ifstream fin(filename.data(), std::ios_base::binary);
			if (fin.is_open())
			{
				fin.seekg(0, std::ios_base::end);
				uint64_t file_size = fin.tellg();
				fin.seekg(0, std::ios_base::beg);

				std::vector<char> data(file_size);
				fin.read(data.data(), file_size);

				rhi::TextureDesc desc;

				char* image_data = nullptr;
				int width = 0;
				int height = 0;
				int channel = 0;

				uint8_t* buf = (uint8_t*)data.data();
				size_t len = data.size();
				if (stbi_is_16_bit_from_memory(buf, len))
				{
					image_data = (char*)stbi_load_16_from_memory(buf, len, &width, &height, &channel, STBI_rgb_alpha);

					// 强制读取RGBA格式,部分图形API不支持RGB纹理,如(DirteX,Vualkan)
					if (channel == 3)
					{
						channel = 4;
					}

					desc.width = width;
					desc.height = height;

					if (channel == 1)
					{
						desc.format = rhi::Format::R16_UNORM;
					}
					else if (channel == 2)
					{
						desc.format = rhi::Format::RG16_UNORM;
					}
					else if (channel == 4)
					{
						desc.format = rhi::Format::RGBA16_UNORM;
					}
				}
				else if (stbi_is_hdr_from_memory(buf, len))
				{
					image_data = (char*)stbi_loadf_from_memory(buf, len, &width, &height, &channel, 0);

					desc.width = width;
					desc.height = height;

					if (channel == 1)
					{
						desc.format = rhi::Format::R32_FLOAT;
					}
					else if (channel == 2)
					{
						desc.format = rhi::Format::RG32_FLOAT;
					}
					else if (channel == 3)
					{
						desc.format = rhi::Format::RGB32_FLOAT;
					}
					else if (channel == 4)
					{
						desc.format = rhi::Format::RGBA32_FLOAT;
					}
				}
				else
				{
					// 强制读取RGBA格式,部分图形API不支持RGB纹理,如(DirteX,Vualkan)
					image_data = (char*)stbi_load_from_memory(buf, len, &width, &height, &channel, STBI_rgb_alpha);

					// 强制读取RGBA格式,部分图形API不支持RGB纹理,如(DirteX,Vualkan)
					if (channel == 3)
					{
						channel = 4;
					}

					desc.width = width;
					desc.height = height;

					if (channel == 1)
					{
						desc.format = rhi::Format::R8_UNORM;
					}
					else if (channel == 2)
					{
						desc.format = rhi::Format::RG8_UNORM;
					}
					else if (channel == 4)
					{
						desc.format = force_srgb ? rhi::Format::SRGBA8_UNORM : rhi::Format::RGBA8_UNORM;
					}
				}

				desc.mip_levels = 1;

				uint8_t bytes_per_pixel = GetFormatInfo(desc.format).bytes_per_pixel;
				std::vector<rhi::TextureSubresourceData> tex_data(desc.mip_levels);
				tex_data[0].data = (char*)image_data;
				tex_data[0].row_pitch = desc.width * bytes_per_pixel;
				tex_data[0].data_size = desc.width * desc.height * bytes_per_pixel;

				tex = Application::Get().GetDevice()->CreateTexture(desc);
				auto command_list = Application::Get().GetDevice()->GetCommandList(rhi::CommandListType::kCopy);
				command_list->WriteTexture(tex, 0, tex_data.size(), tex_data);
				command_list->ExecuteCommandList();
			}
		}

		return tex;
	}
}