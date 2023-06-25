#include "engine/asset/asset.h"
#include "engine/asset/asset_manager.h"
namespace light
{
	std::string Asset::GetUuidString() const
	{
		return uuid.is_nil() ? "" : uuid::ToString(uuid);
	}

	const std::string& Asset::GetFilePath() const
	{
		return AssetManager::GetAssetMeta(uuid).path;
	}
}