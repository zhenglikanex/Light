#include "engine/renderer/shader.h"

namespace light
{
	Shader::Shader(rhi::Shader* vs, rhi::Shader* ps, rhi::Shader* gs)
		: vs_(vs)
		, ps_(ps)
		, gs_(gs)
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
				if (std::find_if(bind_resources_.begin(), bind_resources_.end(), [&bind_resource](const rhi::ShaderBindResourceDeclaration& value)
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
				if (std::find_if(bind_resources_.begin(), bind_resources_.end(), [&bind_resource](const rhi::ShaderBindResourceDeclaration& value)
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
			param_declaractions_.insert(vs->GetParamDeclarations().begin(), vs->GetParamDeclarations().end());
		}

		if (ps_)
		{
			param_declaractions_.insert(ps_->GetParamDeclarations().begin(), ps_->GetParamDeclarations().end());
		}

		if (gs_)
		{
			param_declaractions_.insert(gs_->GetParamDeclarations().begin(), gs_->GetParamDeclarations().end());
		}
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
}