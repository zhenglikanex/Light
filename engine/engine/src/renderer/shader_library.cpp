#include "engine/renderer/shader_library.h"
#include "engine/core/core.h"
#include "engine/core/application.h"
#include "engine/rhi/shader.h"

#include <fstream>
#include <sstream>

const std::string kVsShaderEntryPoint = "VsMain";
const std::string kPsShaderEntryPoint = "PsMain";
const std::string kGsShaderEntryPoint = "GsMain";

namespace light
{
	ShaderLibrary* ShaderLibrary::s_instance = nullptr;

	ShaderLibrary& ShaderLibrary::Get()
	{
		if (!s_instance)
		{
			s_instance = new ShaderLibrary();
		}

		return *s_instance;
	}

	void ShaderLibrary::Init()
	{
		Load("simplepbr", "assets/shaders/simplepbr.hlsl");
		Load("hdr", "assets/shaders/hdr.hlsl");
		Load("shadow", "assets/shaders/shadow.hlsl");
	}

	void ShaderLibrary::Shutdown()
	{
		shaders_.clear();
	}

	Shader* ShaderLibrary::Load(const std::string& name, std::string_view filepath)
	{
		LIGHT_ASSERT(!Exist(name), "shader load failed,shader exist!");
		
		std::fstream fs(filepath.data());

		LIGHT_ASSERT(fs.is_open(), "shader file open failed!");

		std::stringstream sstream;
		sstream << fs.rdbuf();
		
		fs.close();

		std::string source = sstream.str();

		rhi::ShaderHandle vs;
		rhi::ShaderHandle ps;
		rhi::ShaderHandle gs;

		if (FindSubShader(source, rhi::ShaderType::kVertex))
		{
			 vs = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kVertex, filepath, kVsShaderEntryPoint, "vs_5_1");
		}
		
		if (FindSubShader(source, rhi::ShaderType::kPixel))
		{
			ps = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kPixel, filepath, kPsShaderEntryPoint, "ps_5_1");
		}

		if (FindSubShader(source, rhi::ShaderType::kGeometry))
		{
			gs = Application::Get().GetDevice()->CreateShader(rhi::ShaderType::kGeometry, filepath, kGsShaderEntryPoint, "gs_5_1");
		}

		LIGHT_ASSERT(vs && ps, "shader error!");

		Ref<Shader> shader = MakeRef<Shader>(filepath, vs, ps, gs);
		Add(name, shader);

		return shader;
	}

	void ShaderLibrary::Add(std::string_view name, Shader* shader)
	{
		LIGHT_ASSERT(!Exist(std::string(name)), "shader add failed,shader exist!");

		shaders_.emplace(name, shader);
	}

	Shader* ShaderLibrary::Get(const std::string& name)
	{
		LIGHT_ASSERT(Exist(name), "shader not found!");
		return shaders_[name];
	}

	bool ShaderLibrary::FindSubShader(const std::string& source,rhi::ShaderType type)
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

	bool ShaderLibrary::Exist(const std::string& name)
	{
		return shaders_.contains(name);
	}
}