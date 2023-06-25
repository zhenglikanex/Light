#pragma once

#include "engine/asset/asset_manager.h"

#include "engine/light.h"

namespace light::editor
{
	class AssetBrowserPanel
	{
	public:
		AssetBrowserPanel() = default;

		void Init();

		void OnImguiRender();

		void OnEvent(const Event& e);
	private:
		static constexpr uint32_t kIconSize = 100;

		void ImportAsset(const std::filesystem::path& parent_path,const std::filesystem::directory_entry& entry);

		void DrawEntry(const std::filesystem::directory_entry& entry);
		
		std::filesystem::path current_path_;
		std::filesystem::directory_entry select_entry_;

		std::filesystem::directory_entry rename_entry_;

		rhi::TextureHandle folder_icon_;
		rhi::TextureHandle file_icon_;
		rhi::TextureHandle open_icon_;
	};
}