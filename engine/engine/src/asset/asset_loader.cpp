#include "engine/asset/asset_loader.h"
#include "engine/asset/asset_manager.h"

#include "engine/rhi/texture.h"
#include "engine/rhi/command_list.h"

#include "engine/core/application.h"

#include "engine/renderer/mesh.h"
#include "engine/renderer/cube_map.h"

#include "engine/serializer/material_serializer.h"
#include "engine/serializer/cube_map_serializer.h"
#include "engine/utils/string_utils.h"

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
			stbi_set_flip_vertically_on_load(true);
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
		if (path.empty())
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
		static std::string s_PropertiesTag = "#Properties";
		static std::string s_ShaderTag = "#Shader";

		std::string path = AssetManager::GetAssetAbsolutePath(meta.path).string();

		std::fstream fs(path);

		std::stringstream sstream;
		sstream << fs.rdbuf();

		fs.close();

		std::string source = sstream.str();

		Ref<Shader> shader;

		rhi::ShaderHandle vs;
		rhi::ShaderHandle ps;
		rhi::ShaderHandle gs;

		if (source.find(s_PropertiesTag) == std::string::npos)
		{
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
			shader = MakeRef<Shader>(vs, ps, gs);
		}
		else
		{
			uint32_t properties_pos = source.find(s_PropertiesTag);
			uint32_t shader_pos = source.find(s_ShaderTag);
			std::string properties_source(source.data() + properties_pos + s_PropertiesTag.size(), source.data() + shader_pos);

			std::vector<ShaderProperty> properties = ParseShaderProperties(properties_source);

			const char* data = source.data() + shader_pos + s_ShaderTag.size();
			uint32_t size = source.size() - shader_pos - s_ShaderTag.size();

			if (FindSubShader(source, rhi::ShaderType::kVertex))
			{
				vs = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kVertex, path, data, size, kVsShaderEntryPoint, "vs_5_1");
			}

			if (FindSubShader(source, rhi::ShaderType::kPixel))
			{
				ps = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kPixel, path, data, size, kPsShaderEntryPoint, "ps_5_1");
			}

			if (FindSubShader(source, rhi::ShaderType::kGeometry))
			{
				gs = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kGeometry, path, data, size, kGsShaderEntryPoint, "gs_5_1");
			}

			LIGHT_ASSERT(vs && ps, "shader error!");
			shader = MakeRef<Shader>(std::move(properties), vs, ps, gs);
		}

		if (shader)
		{
			shader->uuid = meta.uuid;
		}

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

	const char* ShaderLoader::SkipWhiteSpaces(const char* ch)
	{
		while (*ch)
		{

			if (strncmp(ch, "\r\n", 2) == 0 || strncmp(ch, "\n\r", 2) == 0)
			{
				ch += 2;
			}

			switch (*ch)
			{
			case '\t':
			case '\n':
			case '\v':
			case '\r':
			case '\f':
			case ' ':
				++ch;
			default:
				return ch;
			}
		}

		return ch;
	}

	std::vector<ShaderProperty> ShaderLoader::ParseShaderProperties(const std::string& source)
	{
		std::vector<ShaderProperty> properties;

		const char* ch = source.c_str();

		while (ch && *ch)
		{
			ShaderProperty property;
			ch = SkipWhiteSpaces(ch);

			if (!*ch)
			{
				return properties;
			}

			const char* p = nullptr;
			if (*ch == '_' || *ch >= 'a' && *ch <= 'z' || *ch >= 'A' && *ch <= 'Z')
			{
				p = ch;
				// scan 
				while (*ch == '_' || *ch >= 'a' && *ch <= 'z' || *ch >= 'A' && *ch <= 'Z' || *ch >= '0' && *ch <= '9')
				{
					++ch;
				}
			}

			property.variable_name = std::string(p, ch);

			ch = SkipWhiteSpaces(ch);

			if (*ch++ != '(')
			{
				LIGHT_ASSERT(false, "Shader Properties Parse Error!");
				return properties;
			}

			if (*ch++ != '"')
			{
				LIGHT_ASSERT(false, "Shader Properties Parse Error!");
				return properties;
			}

			p = ch;

			while (*ch != '"')
			{
				++ch;
			}

			property.editor_name = std::string(p, ch++);

			ch = SkipWhiteSpaces(ch);

			if (*ch++ != ',')
			{
				LIGHT_ASSERT(false, "Shader Properties Parse Error!");
				return properties;
			}

			ch = SkipWhiteSpaces(ch);

			if (strncmp(ch, "Color", 5) == 0)
			{
				ch += 5;
				property.type = ShaderPropertyType::kColor;
			}
			else if (strncmp(ch, "2D", 2) == 0)
			{
				ch += 2;
				property.type = ShaderPropertyType::kTexture2D;
			}
			else if (strncmp(ch, "Range(", 6) == 0)
			{
				property.type = ShaderPropertyType::kNumber;

				ch += 6;

				ch = SkipWhiteSpaces(ch);
				const char* first = ch;

				while (*ch == '.' || *ch >= '0' && *ch <= '9')
				{
					++ch;
				}

				const char* end = ch;

				double min = 0;
				auto result = std::from_chars(first, end, min);
				if (result.ec != std::errc())
				{
					LIGHT_ASSERT(false, "Shader Properties Parse Error!");
					return properties;
				}

				ch = SkipWhiteSpaces(ch);

				if (*ch++ != ',')
				{
					LIGHT_ASSERT(false, "Shader Properties Parse Error!");
					return properties;
				}

				ch = SkipWhiteSpaces(ch);

				first = ch;
				while (*ch == '.' || *ch >= '0' && *ch <= '9')
				{
					++ch;
				}

				end = ch;

				double max = 0;
				result = std::from_chars(first, end, max);
				if (result.ec != std::errc())
				{
					LIGHT_ASSERT(false, "Shader Properties Parse Error!");
					return properties;
				}

				ch = SkipWhiteSpaces(ch);

				if (*ch++ != ')')
				{
					LIGHT_ASSERT(false, "Shader Properties Parse Error!");
					return properties;
				}

				property.range.min = min;
				property.range.max = max;
			}

			ch = SkipWhiteSpaces(ch);

			if (*ch++ != ')')
			{
				LIGHT_ASSERT(false, "Shader Properties Parse Error!");
				return properties;
			}

			ch = SkipWhiteSpaces(ch);

			if (*ch++ != '=')
			{
				LIGHT_ASSERT(false, "Shader Properties Parse Error!");
				return properties;
			}

			ch = SkipWhiteSpaces(ch);

			if (property.type == ShaderPropertyType::kColor)
			{
				if (*ch++ != '(')
				{
					LIGHT_ASSERT(false, "Shader Properties Parse Error!");
					return properties;
				}

				for (int i = 0; i < 3; ++i)
				{
					ch = SkipWhiteSpaces(ch);
					p = ch;
					while (*ch == '.' || *ch >= '0' && *ch <= '9')
					{
						++ch;
					}

					float number = 0;
					auto result = std::from_chars(p, ch, number);
					if (result.ec != std::errc())
					{
						LIGHT_ASSERT(false, "Shader Properties Parse Error!");
						return properties;
					}

					property.color[i] = number;

					ch = SkipWhiteSpaces(ch);
					if (*ch != ',' && *ch != ')')
					{
						LIGHT_ASSERT(false, "Shader Properties Parse Error!");
						return properties;
					}

					++ch;
				}
			}
			else if (property.type == ShaderPropertyType::kNumber)
			{
				p = ch;

				while (*ch == '.' || *ch >= '0' && *ch <= '9')
				{
					++ch;
				}

				float number = 0;
				auto result = std::from_chars(p, ch, number);
				if (result.ec != std::errc())
				{
					LIGHT_ASSERT(false, "Shader Properties Parse Error!");
					return properties;
				}

				property.number = number;
			}
			else if (property.type == ShaderPropertyType::kTexture2D)
			{
				if (*ch++ != '"')
				{
					LIGHT_ASSERT(false, "Shader Properties Parse Error!");
					return properties;
				}
				p = ch;
				while (*ch != '"')
				{
					++ch;
				}

				property.texture = std::string(p, ch++);
			}

			ch = SkipWhiteSpaces(ch);

			properties.push_back(property);
		}

		return properties;
	}

	Ref<Asset> CubeMapLoader::Load(const AssetMeta& meta)
	{
		std::filesystem::path path = meta.path;

		Ref<CubeMap> cubemap = MakeRef<CubeMap>();

		CubeMapSerializer cs(cubemap);
		if (cs.DeserializeText(AssetManager::GetAssetAbsolutePath(meta.path).string()))
		{
			return cubemap;
		}

		return nullptr;
	}
}