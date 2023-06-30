#include "engine/renderer/shader.h"

#include "engine/renderer/renderer.h"

namespace light
{
	Shader::Shader(rhi::Shader* vs, rhi::Shader* ps, rhi::Shader* gs)
		: vs_(vs)
		, gs_(gs)
		, ps_(ps)
		, cull_mode_(rhi::CullMode::kBack)
	{
		if (ps_)
		{
			bind_resources_ = ps->GetBindResources();
		}

		if (vs_)
		{
			bind_resources_.reserve(bind_resources_.size() + vs_->GetBindResources().size());

			for (auto& bind_resource : vs_->GetBindResources())
			{
				if (std::ranges::find_if(bind_resources_, [&bind_resource](const rhi::ShaderBindResourceDeclaration& value)
				{
					return value.name == bind_resource.name;
				}) == bind_resources_.end())
				{
					bind_resources_.emplace_back(bind_resource);
				}
			}
		}

		if (gs_)
		{
			bind_resources_.reserve(bind_resources_.size() + gs_->GetBindResources().size());

			for (auto& bind_resource : gs_->GetBindResources())
			{
				if (std::ranges::find_if(bind_resources_, [&bind_resource](const rhi::ShaderBindResourceDeclaration& value)
				{
					return value.name == bind_resource.name;
				}) == bind_resources_.end())
				{
					bind_resources_.emplace_back(bind_resource);
				}
			}
		}

		if (vs_)
		{
			param_declaractions_.insert(vs_->GetParamDeclarations().begin(), vs_->GetParamDeclarations().end());
		}

		if (ps_)
		{
			param_declaractions_.insert(ps_->GetParamDeclarations().begin(), ps_->GetParamDeclarations().end());
		}

		if (gs_)
		{
			param_declaractions_.insert(gs_->GetParamDeclarations().begin(), gs_->GetParamDeclarations().end());
		}

		uint32_t size = 0;
		for (auto& [name, decl] : param_declaractions_)
		{
			size += decl.size;
		}

		params_buffer_.resize(size);

		binding_layout_ = CreateBindingLayout();
	}

	Shader::Shader(std::vector<ShaderProperty> properties, rhi::Shader* vs, rhi::Shader* ps, rhi::Shader* gs)
		: Shader(vs, ps, gs)
	{
		properties_ = std::move(properties);

		for (auto& property : properties_)
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

	void Shader::Set(const std::string& name, rhi::TextureHandle texture)
	{
		textures_[name] = texture;
	}

	void Shader::Set(const std::string& name, rhi::Texture* texture)
	{
		textures_[name] = texture;
	}

	int32_t Shader::FindConstantsBufferBindingIndex(const std::string& name) const
	{
		auto it = constants_buffer_bindings_.find(name);
		if(it != constants_buffer_bindings_.end())
		{
			return static_cast<int32_t>(it->second);
		}

		return -1;
	}

	const ShaderBindingTable* Shader::FindTextureBindingTable(const std::string& name) const
	{
		auto it = texture_binding_tables_.find(name);
		if(it != texture_binding_tables_.end())
		{
			return &it->second;
		}

		return nullptr;
	}

	const ShaderBindingTable* Shader::FindSamplerBindingTable(const std::string& name) const
	{
		auto it = sampler_binding_tables_.find(name);
		if(it != sampler_binding_tables_.end())
		{
			return &it->second;
		}

		return nullptr;
	}

	const rhi::ShaderParamDeclaration* Shader::FindParamDeclaration(const std::string& name) const
	{
		auto it = param_declaractions_.find(name);
		if (it != param_declaractions_.end())
		{
			return &it->second;
		}

		return nullptr;
	}

	rhi::BindingLayoutHandle Shader::CreateBindingLayout() const
	{
		bool has_volatile_tex_range = false;
		uint32_t index = 0;
		rhi::BindingLayout* binding_layout = new rhi::BindingLayout(0);

		for (auto& bind_resource : bind_resources_)
		{
			rhi::BindingParameter bind_param;
			if (bind_resource.type == rhi::ShaderBindResourceType::kConstantBuffer)
			{
				bind_param.InitAsConstantBufferView(bind_resource.bind_point, bind_resource.space);
				binding_layout->Add(index, bind_param);
				constants_buffer_bindings_.emplace(bind_resource.name, index);
				++index;
			}
		}

		uint32_t texture_offsets = 0;
		static std::vector<rhi::BindingParameter::DescriptorRange> texture_ranges;
		texture_ranges.clear();

		for (auto& bind_resource : bind_resources_)
		{
			if (bind_resource.type == rhi::ShaderBindResourceType::kTexture)
			{
				if (texture_ranges.empty()
					|| !(texture_ranges.back().base_shader_register + texture_ranges.back().num_descriptors == bind_resource.bind_point
						&& texture_ranges.back().register_space == bind_resource.space))
				{
					rhi::BindingParameter::DescriptorRange range;
					range.range_type = rhi::DescriptorRangeType::kShaderResourceView;
					range.base_shader_register = bind_resource.bind_point;

					if (bind_resource.bind_count == 0) // dynamic array
					{
						// 只允许存在一个volatile tex range
						LIGHT_ASSERT(has_volatile_tex_range == false, "只允许存在一个volatile tex range!")

							range.is_volatile = true;
						// todo
						range.num_descriptors = 32;
						has_volatile_tex_range = true;
					}
					else
					{
						range.num_descriptors = bind_resource.bind_count;
					}
					range.register_space = bind_resource.space;
					texture_ranges.emplace_back(range);
				}
				else
				{
					// 合并连续纹理绑定
					texture_ranges.back().num_descriptors += bind_resource.bind_count;
				}

				texture_binding_tables_.emplace(bind_resource.name, ShaderBindingTable{ index,texture_offsets++ });
			}
		}

		if (texture_ranges.size())
		{
			rhi::BindingParameter texture_param;
			texture_param.InitAsDescriptorTable(texture_ranges.size(), texture_ranges.data(), rhi::ShaderVisibility::kPixel);

			binding_layout->Add(index++, texture_param);
		}

		uint32_t sampler_offset = 0;
		static std::vector<rhi::BindingParameter::DescriptorRange> sampler_ranges;
		sampler_ranges.clear();

		for (auto& bind_resource : bind_resources_)
		{
			if (bind_resource.type == rhi::ShaderBindResourceType::kSampler)
			{
				if (sampler_ranges.empty() || !(sampler_ranges.back().base_shader_register + sampler_ranges.back().num_descriptors == bind_resource.bind_point
					&& sampler_ranges.back().register_space == bind_resource.space))
				{
					rhi::BindingParameter::DescriptorRange range;
					range.range_type = rhi::DescriptorRangeType::kSampler;
					range.base_shader_register = bind_resource.bind_point;
					range.num_descriptors = bind_resource.bind_count;
					range.register_space = bind_resource.space;
					sampler_ranges.emplace_back(range);
				}
				else
				{
					// 合并连续的Sampler
					sampler_ranges.back().num_descriptors += bind_resource.bind_count;
				}

				sampler_binding_tables_.emplace(bind_resource.name, ShaderBindingTable{ index,sampler_offset++ });
			}
		}

		if (!sampler_ranges.empty())
		{
			rhi::BindingParameter sampler_param;
			sampler_param.InitAsDescriptorTable(sampler_ranges.size(), sampler_ranges.data(), rhi::ShaderVisibility::kPixel);

			binding_layout->Add(index++, sampler_param);
		}

		return rhi::BindingLayoutHandle::Create(binding_layout);
	}
}
