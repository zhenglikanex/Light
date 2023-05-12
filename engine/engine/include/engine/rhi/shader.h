#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "resource.h"
#include "types.h"

namespace light::rhi
{
	struct ShaderBindResourceDeclaration
	{
		std::string name;
		ShaderBindResourceType type;
		uint32_t bind_point = 0;
		uint32_t bind_count = 0;
		uint32_t space;          // Register space
	};

	using ShaderBindResourceDeclarationList = std::vector<ShaderBindResourceDeclaration>;

	struct ShaderParamDeclaration
	{
		std::string name;
		uint32_t offset = 0;
		uint32_t size = 0;
	};

	using ShaderParamDeclarationMap = std::unordered_map<std::string, ShaderParamDeclaration>;

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

		const ShaderBindResourceDeclarationList& GetBindResources() const { return bind_resources_; }
		
		const ShaderParamDeclarationMap& GetParamDeclarations() const { return param_declaractions_; }

	protected:
		ShaderDesc desc_;
		std::vector<char> bytecode_;
		ShaderBindResourceDeclarationList bind_resources_;
		ShaderParamDeclarationMap param_declaractions_;
	};

	using ShaderHandle = Handle<Shader>;
}