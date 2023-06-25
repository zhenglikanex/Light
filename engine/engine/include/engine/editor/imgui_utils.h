#pragma once

#ifdef LIGHT_EDITOR

#include "engine/core/base.h"
#include "engine/asset/asset.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include "imgui_internal.h"

namespace light
{
	class Asset;
}

namespace light::ImGuiEditor
{	
	static const char* kAssetMetaDragId = "ASSET_META";

	Asset* InputAsset(AssetType asset_type,const Asset* asset);
}

#endif

