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
				if (std::find_if(bind_resources_.begin(), bind_resources_.end(), [&bind_resource](const rhi::ShaderBindResourceDesc& value)
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
				if (std::find_if(bind_resources_.begin(), bind_resources_.end(), [&bind_resource](const rhi::ShaderBindResourceDesc& value)
					{
						return value.name == bind_resource.name;
					}) == bind_resources_.end())
				{
					bind_resources_.emplace_back(bind_resource);
				}
			}
		}
	}
}