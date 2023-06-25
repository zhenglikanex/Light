#pragma once

#include "engine/asset/asset.h"

namespace light
{
	struct AssetMeta
	{
		AssetType type = AssetType::kInvalid;
		uuids::uuid uuid;
		std::string path;

		bool IsVaild() const
		{
			return !uuid.is_nil() && type != AssetType::kInvalid;
		}
	};
}