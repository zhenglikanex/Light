#include "engine/renderer/material.h"

namespace light
{
	MaterialInstance::MaterialInstance(Material* material)
		: material_(material)
	{

	}

	Material::Material(Shader* shader)
		: shader_(shader)
	{
		uint32_t size = 0;
		for (auto& [name, decl] : shader_->GetParamDeclarations())
		{
			size += decl.size;
		}

		params_buffer_.resize(size);
	}

	void Material::Set(const std::string& name, rhi::Texture* texture)
	{
		textures_[name] = texture;
	}
}