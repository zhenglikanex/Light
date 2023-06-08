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
		Material() = default;
		explicit Material(Shader* shader);
	
		void SetFilePath(std::string_view filepath) { filepath_ = filepath; }

		void SetShader(Shader* shader);

		const Shader* GetShader() const { return shader_; }

		void SetCullMode(rhi::CullMode cull_mode) { shader_->SetCullMode(cull_mode); }

		template<typename T>
		void Set(const std::string& name, const T& value)
		{
			auto decl = shader_->FindParamDeclaration(name);

			LIGHT_ASSERT(decl,"not param!");

			LIGHT_ASSERT(decl->size == sizeof(T),"type size not match!");

			LIGHT_ASSERT(decl->offset + decl->size <= params_buffer_.size(), "out of range!");

			memcpy(params_buffer_.data() + decl->offset, &value, sizeof(T));
		}

		const std::string& GetFilePath() const { return filepath_; }

		template<typename T>
		T Get(const std::string& name) const
		{
			auto decl = shader_->FindParamDeclaration(name);
			LIGHT_ASSERT(decl, "not param!");
			LIGHT_ASSERT(decl->size == sizeof(T), "type size not match!");
			LIGHT_ASSERT(decl->offset + decl->size <= params_buffer_.size(), "out of range!");
			T value;
			memcpy(&value, params_buffer_.data() + decl->offset, sizeof(T));
			return value;
		}

		void Set(const std::string& name, rhi::Texture* texture);

		void SetParamsBuffer(std::vector<uint8_t> buffer) { params_buffer_ = std::move(buffer); }

		const std::vector<uint8_t>& GetParamsBuffer() const { return params_buffer_; }

		const std::unordered_map<std::string, rhi::TextureHandle>& GetTextures() const { return textures_; };
	private:
		std::string filepath_;
		Ref<Shader> shader_;
		std::vector<uint8_t> params_buffer_;
		std::unordered_map<std::string,rhi::TextureHandle> textures_;
	};

	class MaterialInstance : public RefCounter
	{
	public:
		explicit MaterialInstance(Material* material);
	private:
		Material* material_;
	};
}