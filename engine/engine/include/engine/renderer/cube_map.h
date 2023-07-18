#pragma once

#include "engine/asset/asset.h"
#include "engine/rhi/texture.h"

namespace light
{
	struct CubeMap : public Asset
	{
	public:
		enum class Face
		{
			kRight,
			kLeft,
			kTop,
			kButtom,
			kFront,
			kBack
		};

		static AssetType StaticType()
		{
			return AssetType::kCubeMap;
		}

		static const char* StaticName()
		{
			return "CubeMap";
		}

		AssetType GetAssetType() const override { return AssetType::kCubeMap; }

		rhi::Texture* CreateTextureCube();
		
		union
		{
			UUID faces[6] = {};

			UUID right;
			UUID left;
			UUID top;
			UUID buttom;
			UUID front;
			UUID back;
		};
	private:
		rhi::TextureHandle texture_;
	};
}