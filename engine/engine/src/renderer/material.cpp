#include "engine/renderer/material.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/renderer2d.h"

namespace light
{
	MaterialInstance::MaterialInstance(Material* material)
		: material_(material)
	{
		
	}

	Material::Material(Shader* shader)
	{
		SetShader(shader);	
	}

	void Material::SetShader(Shader* shader)
	{
		if (shader_ == shader)
		{
			return;
		}

		shader_ = shader;

		uint32_t size = 0;
		for (auto& [name, decl] : shader_->GetParamDeclarations())
		{
			size += decl.size;
		}

		params_buffer_.clear();
		params_buffer_.resize(size);
		textures_.clear();

		for (auto& property : shader->GetProperties())
		{
			if (property.type == ShaderPropertyType::kNumber)
			{
				Set(property.variable_name, property.number);
			}
			else if (property.type == ShaderPropertyType::kColor)
			{
				Set(property.variable_name, property.color);
			}
			else if (property.type == ShaderPropertyType::kTexture2D)
			{
				rhi::Texture* texture = Renderer::GetBuiltinTexture(property.texture);
				Set(property.variable_name, texture);
			}
		}
	}

	void Material::Set(const std::string& name, rhi::Texture* texture)
	{
		textures_[name] = texture;
	}

	void Material::Set(const std::string& name, rhi::TextureHandle texture)
	{
		textures_[name] = texture;
	}

	rhi::Texture* Material::Get(const std::string& name) const
	{
		auto it = textures_.find(name);
		if (it != textures_.end())
		{
			return it->second;
		}

		return nullptr;
	}
}