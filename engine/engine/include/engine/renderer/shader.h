#pragma once

#include "light_pch.h"

#include "engine/core/base.h"
#include "engine/asset/asset.h"

#include "engine/rhi/shader.h"
#include "engine/rhi/binding_layout.h"
#include "engine/rhi/texture.h"

#include "glm/glm.hpp"

namespace light
{
	struct ShaderBindingTable
	{
		uint32_t index;
		uint32_t offset;
	};

	enum class ShaderPropertyType
	{
		kNumber,
		kColor,
		kTexture2D,
	};

	struct ShaderProperty
	{
		ShaderPropertyType type = ShaderPropertyType::kNumber;

		union 
		{
			float number;
			glm::vec3 color;
		};
		std::string texture;

		std::string variable_name;
		std::string editor_name;
		
		struct Range
		{
			double max;
			double min;
		};

		Range range;
	};

	class Shader : public Asset
	{
	public:
		static AssetType StaticType()
		{
			return AssetType::kShader;
		}

		static const char* StaticName()
		{
			return "Shader";
		}

		Shader(rhi::Shader* vs,rhi::Shader* ps,rhi::Shader* gs);
		Shader(std::vector<ShaderProperty> properties, rhi::Shader* vs, rhi::Shader* ps, rhi::Shader* gs);

		AssetType GetAssetType() const override { return AssetType::kShader; }

		const ShaderProperty* GetProperty(const std::string& name) const;

		const std::vector<ShaderProperty>& GetProperties() const { return properties_; }

		void SetCullMode(rhi::CullMode cull_mode) { cull_mode_ = cull_mode; }

		void SetDepthEnable(bool depth_enable) { depth_enable_ = depth_enable; }

		void SetDepthFunc(rhi::ComparisonFunc depth_func) { depth_func_ = depth_func; }

		template<typename T>
		void Set(const std::string& name, const T& value)
		{
			auto decl = FindParamDeclaration(name);

			LIGHT_ASSERT(decl, "not param!");

			LIGHT_ASSERT(decl->size == sizeof(T), "type size not match!");

			LIGHT_ASSERT(decl->offset + decl->size <= params_buffer_.size(), "out of range!");

			memcpy(params_buffer_.data() + decl->offset, &value, sizeof(T));
		}

		template<typename T>
		T Get(const std::string& name) const
		{
			auto decl = FindParamDeclaration(name);
			LIGHT_ASSERT(decl, "not param!");
			LIGHT_ASSERT(decl->size == sizeof(T), "type size not match!");
			LIGHT_ASSERT(decl->offset + decl->size <= params_buffer_.size(), "out of range!");
			T value;
			memcpy(&value, params_buffer_.data() + decl->offset, sizeof(T));
			return value;
		}

		void Set(const std::string& name, rhi::TextureHandle texture);
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

		rhi::BindingLayoutHandle GetBindingLayout() const { return binding_layout_; }

		rhi::CullMode GetCullMode() const { return cull_mode_; }

		bool IsDepthEnable() const { return depth_enable_; }

		rhi::ComparisonFunc GetDepthFunc() const { return depth_func_; }

		const std::vector<uint8_t>& GetParamsBuffer() const { return params_buffer_; }

		const std::unordered_map<std::string, rhi::TextureHandle>& GetTextures() const { return textures_; };

		const std::unordered_map<std::string, ShaderBindingTable>& GetSamplerBindingTables() const { return sampler_binding_tables_; }
	private:
		rhi::BindingLayoutHandle CreateBindingLayout() const;

		std::vector<ShaderProperty> properties_;

		rhi::ShaderHandle vs_;
		rhi::ShaderHandle gs_;
		rhi::ShaderHandle ps_;
		rhi::ShaderBindResourceDeclarationList bind_resources_;
		rhi::ShaderParamDeclarationMap param_declaractions_;

		rhi::BindingLayoutHandle binding_layout_;

		mutable std::unordered_map<std::string, uint32_t> constants_buffer_bindings_;
		mutable std::unordered_map<std::string, ShaderBindingTable> texture_binding_tables_;
		mutable std::unordered_map<std::string, ShaderBindingTable> sampler_binding_tables_;

		rhi::CullMode cull_mode_;
		bool depth_enable_;
		rhi::ComparisonFunc depth_func_;
		std::vector<uint8_t> params_buffer_;
		std::unordered_map<std::string, rhi::TextureHandle> textures_;
	};
}
