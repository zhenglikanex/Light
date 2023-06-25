#include "engine/asset/asset_loader.h"
#include "engine/asset/asset_manager.h"

#include "engine/rhi/texture.h"
#include "engine/rhi/command_list.h"

#include "engine/core/application.h"

#include "engine/renderer/mesh.h"

#include "engine/serializer/material_serializer.h"

#include <filesystem>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // !STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

static const std::string kVsShaderEntryPoint = "VsMain";
static const std::string kPsShaderEntryPoint = "PsMain";
static const std::string kGsShaderEntryPoint = "GsMain";

namespace light
{
	Ref<Asset> light::TextureLoader::Load(const AssetMeta& meta)
	{
		// todo 移动到AssetMeta中
		bool force_srgb = false;

		rhi::TextureHandle asset;

		std::filesystem::path path = meta.path;

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
			std::ifstream fin(AssetManager::GetAssetAbsolutePath(meta.path), std::ios_base::binary);
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

				asset = Application::Get().GetDevice()->CreateTexture(desc);
				auto command_list = Application::Get().GetDevice()->GetCommandList(rhi::CommandListType::kCopy);
				command_list->WriteTexture(asset, 0, tex_data.size(), tex_data);
				command_list->ExecuteCommandList();
			}
		}

		if (asset)
		{
			asset->uuid = meta.uuid;
		}

		return asset;
	}

	Ref<Asset> MeshLoader::Load(const AssetMeta& meta)
	{
		std::string path = AssetManager::GetAssetAbsolutePath(meta.path).string();
		if(path.empty())
		{
			return nullptr;
		}
		
		Ref<Asset> asset = MakeRef<Mesh>(path);
		asset->uuid = meta.uuid;

		return asset;
	}

	Ref<Asset> MaterialLoader::Load(const AssetMeta& meta)
	{
		std::string path = AssetManager::GetAssetAbsolutePath(meta.path).string();
		
		Ref<Material> material = MakeRef<Material>();
		MaterialSerializer ms(material);
		ms.DeserializeText(path);
		material->uuid = meta.uuid;

		return material;
	}

	Ref<Asset> ShaderLoader::Load(const AssetMeta& meta)
	{
		std::string path = AssetManager::GetAssetAbsolutePath(meta.path).string();

		std::fstream fs(path);

		std::stringstream sstream;
		sstream << fs.rdbuf();

		fs.close();

		std::string source = sstream.str();

		rhi::ShaderHandle vs;
		rhi::ShaderHandle ps;
		rhi::ShaderHandle gs;

		if (FindSubShader(source, rhi::ShaderType::kVertex))
		{
			vs = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kVertex, path, kVsShaderEntryPoint, "vs_5_1");
		}

		if (FindSubShader(source, rhi::ShaderType::kPixel))
		{
			ps = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kPixel, path, kPsShaderEntryPoint, "ps_5_1");
		}

		if (FindSubShader(source, rhi::ShaderType::kGeometry))
		{
			gs = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kGeometry, path, kGsShaderEntryPoint, "gs_5_1");
		}

		LIGHT_ASSERT(vs && ps, "shader error!");

		Ref<Shader> shader = MakeRef<Shader>(vs, ps, gs);
		shader->uuid = meta.uuid;

		return shader;
	}
	bool ShaderLoader::FindSubShader(const std::string& source, rhi::ShaderType type)
	{
		if (rhi::ShaderType::kVertex == type)
		{
			return source.find(kVsShaderEntryPoint) != std::string::npos;
		}
		else if (rhi::ShaderType::kPixel == type)
		{
			return source.find(kPsShaderEntryPoint) != std::string::npos;
		}
		else if (rhi::ShaderType::kGeometry == type)
		{
			return source.find(kGsShaderEntryPoint) != std::string::npos;
		}

		return false;
	}
}