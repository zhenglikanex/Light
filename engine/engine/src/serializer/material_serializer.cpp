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
			if (material_->GetShader()->GetParamDeclarations().size() > 0)
			{
				out << YAML::Key << "Params" << YAML::BeginSeq;
				for (auto& [name, decl] : material_->GetShader()->GetParamDeclarations())
				{
					out << YAML::BeginMap;

					out << YAML::Key << "Name" << YAML::Value << name;
					out << YAML::Key << "Type" << YAML::Value << decl.type;

					out << YAML::Key << "Value";
					if (decl.type == "float")
					{
						out << YAML::Value << material_->Get<float>(name);
					}
					else if (decl.type == "float2")
					{
						out << YAML::Value << material_->Get<glm::vec2>(name);
					}
					else if (decl.type == "float3")
					{
						out << YAML::Value << material_->Get<glm::vec3>(name);
					}
					else if (decl.type == "float4")
					{
						out << YAML::Value << material_->Get<glm::vec4>(name);
					}
					else if (decl.type == "int")
					{
						out << YAML::Value << material_->Get<int>(name);
					}
					else if (decl.type == "int2")
					{
						out << YAML::Value << material_->Get<glm::ivec2>(name);
					}
					else if (decl.type == "int3")
					{
						out << YAML::Value << material_->Get<glm::ivec3>(name);
					}
					else if (decl.type == "int4")
					{
						out << YAML::Value << material_->Get<glm::ivec4>(name);
					}
					else
					{
						LIGHT_ASSERT(false, std::format("{} : Not Supported Type!", decl.type));
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
			std::string type = node["Type"].as<std::string>();
			if (type == "float")
			{
				float value = node["Value"].as<float>();
				material_->Set(name, value);
			}
			else if (type == "float2")
			{
				glm::vec2 value = node["Value"].as<glm::vec2>();
				material_->Set(name, value);
			}
			else if (type == "float3")
			{
				glm::vec3 value = node["Value"].as<glm::vec3>();
				material_->Set(name, value);
			}
			else if (type == "float4")
			{
				glm::vec4 value = node["Value"].as<glm::vec4>();
				material_->Set(name, value);
			}
			else if (type == "int")
			{
				int value = node["Value"].as<int>();
				material_->Set(name, value);
			}
			else if (type == "int2")
			{
				glm::ivec2 value = node["Value"].as<glm::ivec2>();
				material_->Set(name, value);
			}
			else if (type == "int3")
			{
				glm::ivec3 value = node["Value"].as<glm::ivec3>();
				material_->Set(name, value);
			}
			else if (type == "int4")
			{
				glm::ivec4 value = node["Value"].as<glm::ivec4>();
				material_->Set(name, value);
			}
			else
			{
				LIGHT_ASSERT(false, std::format("{} : Not Supported Type!", type));
			}
		}

		return true;
	}
}