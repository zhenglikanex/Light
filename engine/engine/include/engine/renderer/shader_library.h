#pragma once

#include <string_view>
#include <unordered_map>

#include "engine/rhi/shader.h"

namespace light
{
	class ShaderLibrary
	{
	public:
		void Load(std::string_view name, rhi::ShaderType type, std::string_view file_path);

		void Add(std::string_view name,rhi::Shader* shader);

		rhi::Shader* Get(const std::string& name, rhi::ShaderType type);
	private:
		bool Exist(const std::string& name, rhi::ShaderType type);

		std::unordered_map<std::string, rhi::ShaderHandle> vertex_shaders_;
		std::unordered_map<std::string, rhi::ShaderHandle> pixel_shaders_;
	};
}