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

		void SetParamsBuffer(std::vector<uint8_t> buffer) { params_buffer_ = std::move(buffer); }
		const std::vector<uint8_t>& GetParamsBuffer() const { return params_buffer_; }
	private:
		Ref<Shader> shader_;
		std::vector<uint8_t> params_buffer_;
	};

	class MaterialInstance : public RefCounter
	{
	public:
		explicit MaterialInstance(Material* material);
	private:
		Material* material_;
	};
}