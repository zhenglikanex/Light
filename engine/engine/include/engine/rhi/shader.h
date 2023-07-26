#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "engine/core/base.h"

#include "engine/core/base.h"
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

		uint32_t dimension = 0;      // Dimension (if texture)
	};

	using ShaderBindResourceDeclarationList = std::vector<ShaderBindResourceDeclaration>;

	struct ShaderParamDeclaration
	{
		std::string name;
		std::string type;
		uint32_t offset = 0;
		uint32_t size = 0;
	};

	using ShaderParamDeclarationMap = std::unordered_map<std::string, ShaderParamDeclaration>;

	struct ShaderDesc
	{
		ShaderType type = ShaderType::kNone;
	};

	class Shader : public RefCounter
	{
	public:
		inline static const std::string kSceneDataName = "cbSceneData";
		inline static const std::string kPerDrawConstantsName = "cbPerDrawConstants";
		inline static const std::string kMaterialConstantsName = "cbMaterialConstants";

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

	using ShaderHandle = Ref<Shader>;
}

template<>
struct std::hash<light::rhi::ShaderBindResourceDeclaration>
{
	size_t operator()(const light::rhi::ShaderBindResourceDeclaration& bind_resource) const noexcept
	{
		using namespace light;

		size_t hash = 0;
		HashCombine(hash, static_cast<uint64_t>(bind_resource.type));
		HashCombine(hash, bind_resource.bind_count);
		HashCombine(hash, bind_resource.bind_point);
		HashCombine(hash, bind_resource.space);
		HashCombine(hash, bind_resource.dimension);

		return hash;
	}
};