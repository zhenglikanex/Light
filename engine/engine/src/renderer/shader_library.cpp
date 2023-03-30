#include "engine/renderer/shader_library.h"
#include "engine/core/core.h"
#include "engine/core/application.h"
#include "engine/rhi/shader.h"

namespace light
{
	void ShaderLibrary::Load(std::string_view name, rhi::ShaderType type,std::string_view file_path)
	{
		LIGHT_ASSET(!Exist(std::string(name),type), "shader load failed,shader exist!");

		rhi::ShaderDesc desc;
		desc.type = type;

		std::string entry_point;
		std::string target;
		if (type == rhi::ShaderType::kVertex)
		{
			entry_point = "VS";
			target = "vs_5_0";
		}
		else if(type == rhi::ShaderType::kPixel)
		{
			entry_point = "PS";
			target = "ps_5_0";
		}

		rhi::ShaderHandle shader = Application::Get().GetDevice()->CreateShader(type, file_path, entry_point, target);
		Add(name,shader);
	}

	void ShaderLibrary::Add(std::string_view name, rhi::Shader* shader)
	{
		rhi::ShaderType type = shader->GetDesc().type;

		LIGHT_ASSET(!Exist(std::string(name),type), "shader add failed,shader exist!");

		if (type == rhi::ShaderType::kVertex)
		{
			rhi::ShaderHandle handle = shader;
			vertex_shaders_.emplace(std::string(name), handle);
		}
		else if (type == rhi::ShaderType::kPixel)
		{
			rhi::ShaderHandle handle = shader;
			pixel_shaders_.emplace(std::string(name), handle);
		}
	}

	rhi::Shader* ShaderLibrary::Get(const std::string& name, rhi::ShaderType type)
	{
		LIGHT_ASSET(Exist(name,type), "shader not found!");

		if (type == rhi::ShaderType::kVertex)
		{
			return vertex_shaders_[name];
		}
		else if (type == rhi::ShaderType::kPixel)
		{
			return pixel_shaders_[name];
		}

		return nullptr;
	}

	bool ShaderLibrary::Exist(const std::string& name, rhi::ShaderType type)
	{
		if (type == rhi::ShaderType::kVertex)
		{
			return vertex_shaders_.contains(name);
		}
		else if (type == rhi::ShaderType::kPixel)
		{
			return pixel_shaders_.contains(name);
		}

		return false;
	}
}