#pragma once

#include "light_pch.h"

#include "engine/core/core.h"
#include "engine/renderer/shader.h"

namespace light
{
	class Material : public RefCounter
	{
	public:
		Material(Shader* shader);
		
		Shader* GetShader() const { return shader_; }
	private:
		Ref<Shader> shader_;
	};

	class MaterialInstance : public RefCounter
	{
	public:
		explicit MaterialInstance(Material* material);
	private:
		Material* material_;
	};
}