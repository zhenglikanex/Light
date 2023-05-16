#pragma once

#include "light_pch.h"

#include "engine/core/core.h"

#include "engine/rhi/shader.h"
#include "engine/rhi/binding_layout.h"
#include "engine/rhi/texture.h"

namespace light
{
	struct ShaderBindingTable
	{
		uint32_t index;
		uint32_t offset;
	};

	class Shader : public RefCounter
	{
	public:
		Shader(rhi::Shader* vs,rhi::Shader* ps,rhi::Shader* gs);

		template<typename T>
		void Set(const std::string& name, const T& value)
		{
			auto decl = FindParamDeclaration(name);

			LIGHT_ASSERT(decl, "not param!");

			LIGHT_ASSERT(decl->size == sizeof(T), "type size not match!");

			LIGHT_ASSERT(decl->offset + decl->size <= params_buffer_.size(), "out of range!");

			memcpy(params_buffer_.data() + decl->offset, &value, sizeof(T));
		}

		void Set(const std::string& name, rhi::Texture* texture);

		rhi::Shader* GetVS() const { return vs_; }
		rhi::Shader* GetGS() const { return gs_; }
		rhi::Shader* GetPS() const { return ps_; }

		int32_t FindConstantsBufferBindingIndex(const std::string& name) const;

		const ShaderBindingTable* FindTextureBindingTable(const std::string& name) const;

		const ShaderBindingTable* FindSamplerBindingTable(const std::string& name) const;

		const rhi::ShaderParamDeclaration* FindParamDeclaration(const std::string& name) const;

		const rhi::ShaderBindResourceDeclarationList& GetBindResources() const { return bind_resources_; }

		const rhi::ShaderParamDeclarationMap& GetParamDeclarations() const { return param_declaractions_; }

		rhi::BindingLayoutHandle CreateBindingLayout() const;

		const std::vector<uint8_t>& GetParamsBuffer() const { return params_buffer_; }

		const std::unordered_map<std::string, rhi::TextureHandle>& GetTextures() const { return textures_; };

		const std::unordered_map<std::string, ShaderBindingTable>& GetSamplerBindingTables() const { return sampler_binding_tables_; }
	private:
		rhi::ShaderHandle vs_;
		rhi::ShaderHandle gs_;
		rhi::ShaderHandle ps_;
		rhi::ShaderBindResourceDeclarationList bind_resources_;
		rhi::ShaderParamDeclarationMap param_declaractions_;

		mutable std::unordered_map<std::string, uint32_t> constants_buffer_bindings_;
		mutable std::unordered_map<std::string, ShaderBindingTable> texture_binding_tables_;
		mutable std::unordered_map<std::string, ShaderBindingTable> sampler_binding_tables_;

		std::vector<uint8_t> params_buffer_;
		std::unordered_map<std::string, rhi::TextureHandle> textures_;
	};
}
