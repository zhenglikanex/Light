#pragma once

#include <string_view>
#include <unordered_map>

#include "engine/renderer/shader.h"

namespace light
{
	class ShaderLibrary
	{
	public:
		static ShaderLibrary& Get();

		void Init();

		void Shutdown();

		Shader* Load(const std::string& name, std::string_view fielpath);

		void Add(std::string_view name, Shader* shader);

		Shader* Get(const std::string& name);

		bool Exist(const std::string& name);
	private:
		bool FindSubShader(const std::string& source,rhi::ShaderType type);

		static ShaderLibrary* s_instance;

		std::unordered_map<std::string, Ref<Shader>> shaders_;
	};
}