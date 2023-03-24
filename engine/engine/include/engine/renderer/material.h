#pragma once

#include "engine/rhi/shader.h"

namespace light
{
	class Material
	{
	public:

	private:
		rhi::ShaderHandle vs_;
		rhi::ShaderHandle gs_;
		rhi::ShaderHandle ps_;
	};

	class MaterialInstance
	{
	public:
		explicit MaterialInstance(Material* material);
	private:
		Material* material_;
	};
}