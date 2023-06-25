#include "engine/asset/asset_manager.h"

#include <fstream>

#include "engine/event/windows_event.h"
#include "engine/serializer/material_serializer.h"

#include "yaml-cpp/yaml.h"

#undef CreateDirectory

namespace light
{
	const char* kMetaExt = ".meta";

	std::filesystem::path AssetManager::s_asset_path_;
	std::unordered_map<std::string, AssetMeta> AssetManager::s_asset_meta_by_path_map_;
	std::unordered_map<UUID, AssetMeta> AssetManager::s_asset_meta_by_uuid_map_;
	std::unordered_map<UUID, Ref<Asset>> AssetManager::s_loaded_assets_;
	std::array<AssetLoader*, static_cast<uint32_t>(AssetType::kInvalid)> AssetManager::s_asset_loaders_;

	void AssetManager::Init()
	{
		s_asset_loaders_[static_cast<uint32_t>(AssetType::kTexture)] = new TextureLoader();
		s_asset_loaders_[static_cast<uint32_t>(AssetType::kMesh)] = new MeshLoader();
		s_asset_loaders_[static_cast<uint32_t>(AssetType::kMaterial)] = new MaterialLoader();
		s_asset_loaders_[static_cast<uint32_t>(AssetType::kShader)] = new ShaderLoader();

		s_asset_path_ = std::filesystem::current_path() / "assets";

		LoadAllAssetMetaFile();
		ImportAllAsset();
	}

	void AssetManager::Shutdown()
	{
		// todo
	}

	std::filesystem::path AssetManager::GetAssetAbsolutePath(const std::filesystem::path& path)
	{
		return s_asset_path_ / path;
	}

	UUID AssetManager::GetAssetUUID(const std::string& path)
	{
		auto it = s_asset_meta_by_path_map_.find(path);
		if (it != s_asset_meta_by_path_map_.end())
		{
			return it->second.uuid;
		}

		return UUID();
	}

	Ref<Asset> AssetManager::LoadAsset(const AssetMeta& meta)
	{
		if (s_loaded_assets_.find(meta.uuid) != s_loaded_assets_.end())
		{
			return s_loaded_assets_[meta.uuid];
		}

		AssetLoader* loader = s_asset_loaders_[static_cast<uint32_t>(meta.type)];
		Ref<Asset> asset = loader->Load(meta);

		if (!asset)
		{
			LOG_ENGINE_WARN("Asset Load Failed!");
			return nullptr;
		}
		s_loaded_assets_[meta.uuid] = asset;

		return asset;
	}

	const AssetMeta& AssetManager::GetAssetMeta(UUID uuid)
	{
		static AssetMeta empty;

		auto it = s_asset_meta_by_uuid_map_.find(uuid);
		if (it != s_asset_meta_by_uuid_map_.end())
		{
			return it->second;
		}
		return empty;
	}

	const AssetMeta& AssetManager::GetAssetMeta(const std::string& path)
	{
		static AssetMeta empty;

		auto it = s_asset_meta_by_path_map_.find(path);
		if (it != s_asset_meta_by_path_map_.end())
		{
			return it->second;
		}

		return empty;
	}

	std::optional<std::filesystem::directory_entry> AssetManager::CreateAsset(AssetType type, const std::filesystem::path& path, std::string_view filename)
	{
		if (type == AssetType::kMaterial)
		{
			std::string ext = ToExtString(type);

			std::filesystem::path full_path = path / filename;
			full_path.replace_extension(ext);

			if (full_path.is_relative())
			{
				full_path = GetAssetAbsolutePath(full_path.generic_string());
			}

			if (std::filesystem::exists(full_path))
			{
				return {};
			}

			Ref<Material> material = MakeRef<Material>();
			MaterialSerializer ms(material);
			ms.SerializeText(full_path.string());

			ImportAsset(GetAssetRelativePath(full_path));

			return std::make_optional<std::filesystem::directory_entry>(full_path);
		}

		// todo 等待支持
		LIGHT_ASSERT(false,"not support");
		return {};
	}

	void AssetManager::DeleteAsset(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			return;
		}

		std::filesystem::remove(path);

		std::filesystem::path meta_path = path;
		meta_path += kMetaExt;
		if (std::filesystem::exists(meta_path))
		{
			std::filesystem::remove(meta_path);
		}
		
		const AssetMeta& meta = GetAssetMeta(GetAssetRelativePath(path).generic_string());
		if (meta.IsVaild())
		{
			s_asset_meta_by_path_map_.erase(meta.path);
			s_asset_meta_by_uuid_map_.erase(meta.uuid);
		}
	}

	void AssetManager::RenameAsset(const std::filesystem::path& path, std::string_view new_filename)
	{
		std::filesystem::path relative_path = GetAssetRelativePath(path);
		AssetMeta meta = GetAssetMeta(relative_path.generic_string());
		if (!meta.IsVaild())
		{
			return;
		}
		
		std::filesystem::path new_path = path;
		new_path.replace_filename(new_filename);

		if (!new_path.has_extension())
		{
			new_path += ToExtString(meta.type);
		}

		std::error_code err;
		std::filesystem::rename(path, new_path, err);
		if (err)
		{
			LOG_ENGINE_ERROR("RenameAsset Failed :{}", err.message());

			return;
		}

		std::filesystem::path old_meta_path = path;
		old_meta_path += kMetaExt;
		
		std::filesystem::path new_meta_path = new_path;
		new_meta_path += kMetaExt;

		std::filesystem::rename(old_meta_path, new_meta_path, err);
		if (err)
		{
			LOG_ENGINE_ERROR("RenameAsset Failed :{}", err.message());
			return;
		}

		meta.path = new_path.generic_string();

		s_asset_meta_by_uuid_map_[meta.uuid] = meta;
		s_asset_meta_by_path_map_[meta.path] = meta;

		SerializeAssetMeta(meta, new_meta_path.string());
	}

	void AssetManager::CreateDirectory(const std::filesystem::path& path, std::string_view directory_name)
	{
		// todo
	}

	void AssetManager::DeleteDirectory(const std::filesystem::path& path)
	{
		// todo
	}

	void AssetManager::RenameDirectory(const std::filesystem::path& path)
	{
		// todo
	}

	void AssetManager::LoadAllAssetMetaFile()
	{
		std::filesystem::recursive_directory_iterator iter(s_asset_path_);
		for (const auto& entry : iter)
		{	
			if (!entry.is_directory())
			{
				std::string ext = entry.path().extension().string();
				if (ext == kMetaExt)
				{
					// TODO : 移动到专门的序列化类中
					std::fstream fs(entry.path());
					
					std::stringstream sstream;
					sstream << fs.rdbuf();

					YAML::Node data = YAML::Load(sstream.str());

					if (!data["Meta"])
					{
						continue;
					}

					AssetMeta asset_meta;

					YAML::Node meta = data["Meta"];
					asset_meta.uuid = UUID::from_string((meta["Assetuuid"].as<std::string>())).value();
					asset_meta.path = meta["AssetPath"].as<std::string>();
					asset_meta.type = static_cast<AssetType>(meta["AssetType"].as<uint32_t>());

					if (std::filesystem::exists(s_asset_path_ / asset_meta.path))
					{
						s_asset_meta_by_path_map_.emplace(asset_meta.path, asset_meta);
						s_asset_meta_by_uuid_map_.emplace(asset_meta.uuid, asset_meta);
					}
					else
					{
						LOG_ENGINE_WARN("Asset Miss:File Path {},Meta Path {}", asset_meta.path, entry.path().generic_string());
					}
				}
			}
		}
	}

	void AssetManager::ImportAllAsset()
	{
		std::filesystem::recursive_directory_iterator iter(s_asset_path_);
		for (const auto& entry : iter)
		{
			if (!entry.is_directory())
			{
				std::string ext = entry.path().extension().string();

				if (ext != kMetaExt)
				{
					std::filesystem::path path = GetAssetRelativePath(entry.path());
					if (!s_asset_meta_by_path_map_.contains(path.generic_string()))
					{
						ImportAsset(path);
					}
				}
			}
		}
	}

	bool AssetManager::ImportAsset(const std::filesystem::path& path)
	{
		std::string ext = path.extension().string();
		AssetType type = ToAssetType(ext);
		
		if (type == AssetType::kInvalid)
		{
			LOG_ENGINE_WARN("Asset Import Failed,Not Support AssetType : {}", path.generic_string());
			return false;
		}

		AssetMeta meta;
		meta.type = type;
		meta.uuid = uuid::Gen();
		meta.path = path.generic_string();

		s_asset_meta_by_path_map_.emplace(meta.path, meta);
		s_asset_meta_by_uuid_map_.emplace(meta.uuid, meta);

		std::filesystem::path meta_path = path;
		meta_path.replace_extension(ext + kMetaExt);

		return SerializeAssetMeta(meta,GetAssetAbsolutePath(meta_path).string());
	}

	std::filesystem::path AssetManager::GetAssetRelativePath(const std::filesystem::path& path)
	{
		std::string path_str = path.generic_string();
		std::string current_path = s_asset_path_.generic_string();
		size_t pos = path_str.find(current_path);
		if (pos != std::string::npos && pos == 0)
		{			
			std::string_view path_str_view = path_str;
			path_str_view = path_str_view.substr(pos + current_path.size() + 1);

			return std::filesystem::path(path_str_view);
		}

		return path;
	}

	bool AssetManager::SerializeAssetMeta(const AssetMeta& meta, const std::string& path)
	{
		// TODO : 移动到专门的序列化类中
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Meta" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "AssetType" << YAML::Value << static_cast<uint32_t>(meta.type);
		out << YAML::Key << "Assetuuid" << YAML::Value << uuids::to_string(meta.uuid);
		out << YAML::Key << "AssetPath" << YAML::Value << meta.path;
		out << YAML::EndMap;
		out << YAML::EndMap;

		std::ofstream fs(path);
		if (!fs)
		{
			LOG_ENGINE_WARN("Asset Meta File Craete Failed : {}", path);
			return false;
		}

		fs << out.c_str();

		return true;
	}
}