#pragma once

#include "engine/core/core.h"

#include "engine/rhi/shader.h"

namespace light
{
	class Shader : public RefCounter
	{
	public:
		Shader(rhi::Shader* vs,rhi::Shader* ps,rhi::Shader* gs);

		rhi::Shader* GetVS() const { return vs_; }
		rhi::Shader* GetGS() const { return gs_; }
		rhi::Shader* GetPS() const { return ps_; }

		const rhi::ShaderParamDeclaration* FindParamDeclaration(const std::string& name) const;

		const rhi::ShaderBindResourceDeclarationList& GetBindResources() const { return bind_resources_; }

		const rhi::ShaderParamDeclarationMap& GetParamDeclarations() const { return param_declaractions_; }
	private:
		rhi::ShaderHandle vs_;
		rhi::ShaderHandle gs_;
		rhi::ShaderHandle ps_;

		rhi::ShaderBindResourceDeclarationList bind_resources_;
		rhi::ShaderParamDeclarationMap param_declaractions_;
	};
}