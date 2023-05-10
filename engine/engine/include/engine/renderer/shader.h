#pragma once

#include "engine/core/core.h"

#include "engine/rhi/shader.h"

namespace light
{
	class Shader : public RefCounter
	{
	public:
		Shader(std::string_view file);

		rhi::Shader* GetVS() const { return vs_; }
		rhi::Shader* GetGS() const { return gs_; }
		rhi::Shader* GetPS() const { return ps_; }
	private:
		rhi::ShaderHandle vs_;
		rhi::ShaderHandle gs_;
		rhi::ShaderHandle ps_;
	};
}