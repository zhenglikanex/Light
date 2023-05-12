#pragma once

#include "light_pch.h"

#include "engine/core/core.h"
#include "engine/renderer/shader.h"

#include "engine/rhi/texture.h"

namespace light
{
	class Material : public RefCounter
	{
	public:
		Material(Shader* shader);
		
		Shader* GetShader() const { return shader_; }

		template<typename T>
		void Set(const std::string& name, const T& value)
		{
			auto decl = shader_->FindParamDeclaration(name);

			LIGHT_ASSERT(decl,"not param!");

			LIGHT_ASSERT(decl->size == sizeof(T),"type size not match!");

			LIGHT_ASSERT(decl->offset + decl->size <= params_buffer_.size(), "out of range!");

			memcpy(params_buffer_.data() + decl->offset, &value, sizeof(T));
		}

		void Set(const std::string& name, rhi::Texture* texture);

		void SetParamsBuffer(std::vector<uint8_t> buffer) { params_buffer_ = std::move(buffer); }
		const std::vector<uint8_t>& GetParamsBuffer() const { return params_buffer_; }
	private:
		Ref<Shader> shader_;
		std::vector<uint8_t> params_buffer_;
		std::vector<rhi::Texture> textures_;
	};

	class MaterialInstance : public RefCounter
	{
	public:
		explicit MaterialInstance(Material* material);
	private:
		Material* material_;
	};
}