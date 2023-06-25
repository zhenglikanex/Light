#pragma once

#include "engine/core/base.h"
#include "engine/core/uuid.h"

namespace light
{
	enum class AssetType : uint8_t
	{
		kScene,
		kTexture,
		kMesh,
		kMaterial,
		kShader,
		kInvalid
	};

	inline AssetType ToAssetType(std::string_view ext)
	{
		if (ext == ".scene")
		{
			return AssetType::kScene;
		}
		else if (ext == ".png" || ext == ".jpg")
		{
			return AssetType::kTexture;
		}
		else if (ext == ".obj" || ext == ".fbx")
		{
			return AssetType::kMesh;
		}
		else if (ext == ".mtl")
		{
			return AssetType::kMaterial;
		}
		else if (ext == ".shader")
		{
			return AssetType::kShader;
		}

		return AssetType::kInvalid;
	}

	inline std::string ToExtString(AssetType asset)
	{
		switch (asset)
		{
		case AssetType::kScene:
			return ".scene";
		case AssetType::kTexture:
			return ".png";
		case AssetType::kMesh:
			return ".obj";
		case AssetType::kMaterial:
			return ".mtl";
		case AssetType::kShader:
			return ".shader";
		default:
			LIGHT_ASSERT(false, "Invaild Type");
			return "";
		}
	}

	class Asset : public RefCounter
	{
	public:
		uuids::uuid uuid;

		Asset() = default;

		virtual ~Asset() = default;

		virtual AssetType GetAssetType() const = 0 { return AssetType::kInvalid; }

		std::string GetUuidString() const;

		const std::string& GetFilePath() const;
	};
}