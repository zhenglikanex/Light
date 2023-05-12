#pragma once

#include <vector>
#include <string>

#include "resource.h"
#include "types.h"

namespace light::rhi
{
	struct ShaderBindResourceDesc
	{
		std::string name;
		ShaderBindResourceType type;
		uint32_t bind_point = 0;
		uint32_t bind_count = 0;
		uint32_t space;          // Register space
	};

	struct ShaderDesc
	{
		ShaderType type = ShaderType::kNone;
	};

	class Shader : public Resource
	{
	public:
		Shader(const ShaderDesc& desc, std::vector<char> bytecode)
			: desc_(desc)
			, bytecode_(std::move(bytecode))
		{
		}

		const ShaderDesc& GetDesc() const { return desc_; }

		const std::vector<char>& GetBytecode() const { return bytecode_; }

		const std::vector<ShaderBindResourceDesc>& GetBindResources() const { return bind_resources_; }
	protected:
		ShaderDesc desc_;
		std::vector<char> bytecode_;
		std::vector<ShaderBindResourceDesc> bind_resources_;
	};

	using ShaderHandle = Handle<Shader>;
}