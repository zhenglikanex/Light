#pragma once

#include <string>

#include "engine/core/base.h"
#include "engine/asset/asset.h"
#include "types.h"

namespace light::rhi
{
	struct TextureDesc
	{
		uint32_t width = 1;
		uint32_t height = 1;
		uint32_t depth = 1;
		uint32_t array_size = 1;
		uint32_t mip_levels = 1;
		Format format = Format::UNKNOWN;
		TextureDimension dimension = TextureDimension::kTexture2D;
		bool is_render_target = false;
		std::string debug_name;
	};

	class Texture : public Asset
	{
	public:
		explicit Texture(const TextureDesc& desc)
			: desc_(desc)
			, permanent_state_(false)
		{

		}

		AssetType GetAssetType() const override { return AssetType::kTexture; }

		const TextureDesc& GetDesc() const { return desc_; }

		void SetPermanentState(bool value) { permanent_state_ = value; }

		bool IsPermanentState() const { return permanent_state_; }

		virtual const ClearValue* GetClearValue() const = 0;

		virtual void* GetTextureID() = 0;
	protected:
		TextureDesc desc_;
		bool permanent_state_;
	};

	using TextureHandle = Ref<Texture>;
}