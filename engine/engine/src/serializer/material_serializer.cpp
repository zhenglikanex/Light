#include "engine/serializer/material_serializer.h"
#include "engine/serializer/serializer_utils.h"
#include "engine/asset/asset_manager.h"

#include <format>
#include <fstream>

namespace light
{
	MaterialSerializer::MaterialSerializer(Material* material)
		: material_(material)
	{

	}

	void MaterialSerializer::SerializeText(const std::string& filepath)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;
		out << YAML::Key << "Material" << YAML::Value << filepath;
		out << YAML::Key << "Shader" << YAML::Value << (material_->GetShader() ? material_->GetShader()->GetUuidString() : "");

		if (material_->GetShader())
		{
			if (material_->GetShader()->GetProperties().size() > 0)
			{
				out << YAML::Key << "Params" << YAML::BeginSeq;
				for (auto& property : material_->GetShader()->GetProperties())
				{
					out << YAML::BeginMap;

					out << YAML::Key << "Name" << YAML::Value << property.editor_name;
					out << YAML::Key << "Type" << YAML::Value << static_cast<uint32_t>(property.type);

					out << YAML::Key << "Value";
					if (property.type == ShaderPropertyType::kNumber)
					{
						out << YAML::Value << material_->Get<float>(property.variable_name);
					}
					else if(property.type == ShaderPropertyType::kColor) 
					{
						out << YAML::Value << material_->Get<glm::vec3>(property.variable_name);
					}
					else if (property.type == ShaderPropertyType::kTexture2D)
					{
						out << YAML::Value << material_->Get(property.variable_name)->GetUuidString();
					}
					else 
					{
						LIGHT_ASSERT(false, std::format("{} : Not Supported Type!",static_cast<uint32_t>(property.type)));
					}

					out << YAML::EndMap;
				}

				out << YAML::EndSeq;
			}
		}

		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool MaterialSerializer::DeserializeText(const std::string& filepath)
	{
		std::ifstream fin(filepath);
		std::stringstream sstream;
		sstream << fin.rdbuf();

		YAML::Node data = YAML::Load(sstream.str());

		if (!data["Material"])
			return false;
		
		std::string uuid = data["Shader"].as<std::string>();

		if (uuid.empty())
		{
			return true;
		}

		std::optional<UUID> result = uuid::FromString(uuid);
		if (!result.has_value())
		{
			LOG_ENGINE_WARN("Material Shader Miss : {}", filepath);
			return false;
		}

		Ref<Shader> shader = AssetManager::LoadAsset<Shader>(result.value());
		if (!shader)
		{
			LOG_ENGINE_WARN("Material Shader Miss : {}", filepath);
			return false;
		}

		material_->SetShader(shader);
		
		for (const auto& node : data["Params"])
		{
			std::string name = node["Name"].as<std::string>();
			const ShaderProperty* property = shader->GetProperty(name);
			if (!property)
			{
				continue;
			}

			std::string var_name = property->variable_name;

			ShaderPropertyType type = static_cast<ShaderPropertyType>(node["Type"].as<uint32_t>());
			if (type == ShaderPropertyType::kNumber)
			{
				float value = node["Value"].as<float>();
				material_->Set(var_name, value);
			}
			else if (type == ShaderPropertyType::kColor)
			{
				glm::vec3 value = node["Value"].as<glm::vec3>();
				
				material_->Set(var_name, value);
			}
			else if (type == ShaderPropertyType::kTexture2D)
			{
				std::string value = node["Value"].as<std::string>();
				if (!value.empty())
				{
					auto result = uuid::FromString(value);
					if (result)
					{
						rhi::Texture* texture = AssetManager::LoadAsset<rhi::Texture>(value);
						material_->Set(var_name, texture);
					}
					else
					{
						LOG_ENGINE_WARN("Material Textures Miss : {} ,Texture Name : {}", filepath, name);
					}
				}
			}
			else
			{
				LIGHT_ASSERT(false, std::format("{} : Not Supported Type!", static_cast<uint32_t>(type)));
			}
		}

		return true;
	}
}