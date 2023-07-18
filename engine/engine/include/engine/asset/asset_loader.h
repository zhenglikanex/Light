#pragma once

#include "engine/asset/asset.h"
#include "engine/asset/asset_meta.h"
#include "engine/renderer/shader.h"

#include "engine/rhi/shader.h"

namespace light
{
	class AssetLoader
	{
	public:
		AssetLoader() = default;

		virtual ~AssetLoader() = 0 { }

		virtual Ref<Asset> Load(const AssetMeta& meta) = 0;
	};

	class TextureLoader : public AssetLoader
	{
	public:
		TextureLoader() = default;
		Ref<Asset> Load(const AssetMeta& meta) override;
	};

	class MeshLoader : public AssetLoader
	{
	public:
		Ref<Asset> Load(const AssetMeta& meta) override;
	};

	class MaterialLoader : public AssetLoader
	{
	public:
		Ref<Asset> Load(const AssetMeta& meta) override;
	};

	class ShaderLoader : public AssetLoader
	{
	public:
		Ref<Asset> Load(const AssetMeta& meta) override;

	private:
		bool FindSubShader(const std::string& source, rhi::ShaderType type);

		std::vector<ShaderProperty> ParseShaderProperties(const std::string& source);

		const char* SkipWhiteSpaces(const char* ch);
	};

	class CubeMapLoader : public AssetLoader
	{
	public:
		Ref<Asset> Load(const AssetMeta& meta) override;
	};
}