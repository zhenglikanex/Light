#pragma once

#include <filesystem>
#include <optional>

#include "engine/core/base.h"
#include "engine/asset/asset.h"
#include "engine/asset/asset_meta.h"
#include "engine/asset/asset_loader.h"

#undef CreateDirectory

namespace light
{
	class Event;

	class AssetManager
	{
	public:
		static void Init();
		static void Shutdown();

		static std::filesystem::path GetAssetPath() { return s_asset_path_; }

		static std::filesystem::path GetAssetAbsolutePath(const std::filesystem::path& path);

		static std::filesystem::path GetAssetRelativePath(const std::filesystem::path& path);

		static UUID GetAssetUUID(const std::string& path);

		static bool HasAsset(const std::string& path);

		template<typename T> requires(std::is_base_of_v<Asset, T>)
			static Ref<T> LoadAsset(UUID uuid)
		{
			const AssetMeta& meta = GetAssetMeta(uuid);
			if (meta.uuid.is_nil())
			{
				LOG_ENGINE_WARN("Asset Load Fialed!");
				return nullptr;
			}

			return CastRef<T>(LoadAsset(meta));
		}
		
		template<typename T> requires(std::is_base_of_v<Asset, T>)
			static Ref<T> LoadAsset(const std::string& path)
		{
			const AssetMeta& meta = GetAssetMeta(path);
			if (meta.uuid.is_nil())
			{
				LOG_ENGINE_WARN("Asset Load Fialed!");
				return nullptr;
			}

			return CastRef<T>(LoadAsset(meta));
		}

		static Ref<Asset> LoadAsset(const AssetMeta& meta);

		static const AssetMeta& GetAssetMeta(UUID uuid);
		static const AssetMeta& GetAssetMeta(const std::string& path);

		static std::optional<std::filesystem::directory_entry> CreateAsset(AssetType type,const std::filesystem::path& path,std::string_view filename);
		static void DeleteAsset(const std::filesystem::path& path);
		static void RenameAsset(const std::filesystem::path& path,std::string_view new_filename);

		static void CreateDirectory(const std::filesystem::path& path, std::string_view directory_name);
		static void DeleteDirectory(const std::filesystem::path& path);
		static void RenameDirectory(const std::filesystem::path& path);

		static bool ImportAsset(const std::filesystem::path& path);
	private:
		static void LoadAllAssetMetaFile();
		static void ImportAllAsset();
		
		
		static bool SerializeAssetMeta(const AssetMeta& meta,const std::string& path);

		static std::filesystem::path s_asset_path_;
		static std::unordered_map<std::string, AssetMeta> s_asset_meta_by_path_map_;
		static std::unordered_map<UUID, AssetMeta> s_asset_meta_by_uuid_map_;
		static std::unordered_map<UUID, Ref<Asset>> s_loaded_assets_;
		static std::array<AssetLoader*, static_cast<uint32_t>(AssetType::kInvalid)> s_asset_loaders_;
	};
}