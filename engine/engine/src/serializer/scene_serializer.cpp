#include "engine/serializer/scene_serializer.h"
#include "engine/serializer/serializer_utils.h"
#include "engine/scene/entity.h"
#include "engine/scene/components.h"
#include "engine/reflection/meta.h"
#include <fstream>

namespace light
{
	static void SerializeAny(YAML::Emitter& out,meta::Any& instance)
	{
		out << YAML::BeginMap; 
		out << YAML::Key << "Type" << YAML::Value << instance.GetType().GetName();
		out << YAML::Key << "Fields" << YAML::Value << YAML::BeginMap; // field

		const meta::Method& method = instance.GetType().GetMethod("SerializeText");
		if (method.IsValid())
		{
			method.Invoke(instance, &out);
		}
		else
		{
			for (auto& field : instance.GetType().GetFields())
			{
				out << YAML::Key << field.GetName().data();

				meta::Type type = field.GetType();

				LIGHT_ASSERT(type.IsValid(), "unkown type");

				if (type == meta::Type::Get<bool>())
				{
					out << YAML::Value << field.GetValue(instance).Cast<bool>();
				}
				else if (type == meta::Type::Get<int8_t>())
				{
					out << YAML::Value << field.GetValue(instance).Cast<int8_t>();
				}
				else if (type == meta::Type::Get<uint8_t>())
				{
					out << YAML::Value << field.GetValue(instance).Cast<uint8_t>();
				}
				else if (type == meta::Type::Get<int16_t>())
				{
					out << YAML::Value << field.GetValue(instance).Cast<int16_t>();
				}
				else if (type == meta::Type::Get<uint16_t>())
				{
					out << YAML::Value << field.GetValue(instance).Cast<uint16_t>();
				}
				else if (type == meta::Type::Get<int32_t>())
				{
					out << YAML::Value << field.GetValue(instance).Cast<int32_t>();
				}
				else if (type == meta::Type::Get<uint32_t>())
				{
					out << YAML::Value << field.GetValue(instance).Cast<uint32_t>();
				}
				else if (type == meta::Type::Get<int64_t>())
				{
					out << YAML::Value << field.GetValue(instance).Cast<int64_t>();
				}
				else if (type == meta::Type::Get<uint64_t>())
				{
					out << YAML::Value << field.GetValue(instance).Cast<uint64_t>();
				}
				else if (type == meta::Type::Get<float>())
				{
					out << YAML::Value << field.GetValue(instance).Cast<float>();
				}
				else if (type == meta::Type::Get<double>())
				{
					out << YAML::Value << field.GetValue(instance).Cast<double>();
				}
				else if (type == meta::Type::Get<std::string>())
				{
					out << YAML::Value << field.GetRefValue(instance).Cast<std::string>();
				}
				else if (type == meta::Type::Get<glm::mat4>())
				{
					auto& value = field.GetRefValue(instance).Cast<glm::mat4>();
					out << YAML::Flow;
					out << YAML::Value << YAML::BeginSeq;
					for (int col = 0; col < 4; ++col)
					{
						for (int row = 0; row < 4; ++row)
							out << YAML::Value << value[col][row];
					}
					out << YAML::EndSeq;
				}
				else if (type == meta::Type::Get<glm::mat3>())
				{
					auto& value = field.GetRefValue(instance).Cast<glm::mat3>();
					out << YAML::Flow;
					out << YAML::Value << YAML::BeginSeq;
					for (int col = 0; col < 3; ++col)
					{
						for (int row = 0; row < 3; ++row)
							out << YAML::Value << value[col][row];
					}
					out << YAML::EndSeq;
				}
				else if (type == meta::Type::Get<glm::vec2>())
				{
					auto& value = field.GetRefValue(instance).Cast<glm::vec2>();
					out << YAML::Flow;
					out << YAML::Value << YAML::BeginSeq;
					for (int i = 0; i < 2; ++i)
					{
						out << YAML::Value << value[i];
					}
					out << YAML::EndSeq;
				}
				else if (type == meta::Type::Get<glm::vec3>())
				{
					auto& value = field.GetRefValue(instance).Cast<glm::vec3>();
					out << YAML::Flow;
					out << YAML::Value << YAML::BeginSeq;
					for (int i = 0; i < 3; ++i)
					{
						out << YAML::Value << value[i];
					}
					out << YAML::EndSeq;
				}
				else if (type == meta::Type::Get<glm::vec4>())
				{
					auto& value = field.GetRefValue(instance).Cast<glm::vec4>();
					out << YAML::Flow;
					out << YAML::Value << YAML::BeginSeq;
					for (int i = 0; i < 4; ++i)
					{
						out << YAML::Value << value[i];
					}
					out << YAML::EndSeq;
				}
				else if (type.IsEnum())
				{
					for (auto& inst : type.GetEnumValues())
					{
						if (inst.GetValue() == field.GetValue(instance).Cast<int64_t>())
						{
							out << YAML::Value << inst.GetName().data();
							break;
						}
					}
				}
				else if (type.IsArray())
				{
					auto value = field.GetRefValue(instance);
					out << YAML::Value << YAML::BeginSeq;
					for (int i = 0; i < value.GetSize(); ++i)
					{
						auto element = value.GetElement(i);
						SerializeAny(out, element);
					}
					out << YAML::EndSeq;
				}
				else
				{
					auto value = field.GetRefValue(instance);
					SerializeAny(out, value);
				}
			}
		}

		

		out << YAML::EndMap; // field
		out << YAML::EndMap;

	}

	static void DeserializeAny(YAML::Node data, meta::Any& instance)
	{
		LIGHT_ASSERT(!data[instance.GetType().GetName()],"type error");

		data = data["Fields"];
		const meta::Method& method = instance.GetType().GetMethod("DeserializeText");

		if (method.IsValid())
		{
			method.Invoke(instance, data);
		}
		else {
			for (auto& field : instance.GetType().GetFields())
			{
				meta::Type type = field.GetType();

				LIGHT_ASSERT(type.IsValid(), "unkown type");

				if (type == meta::Type::Get<bool>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<bool>());
				}
				else if (type == meta::Type::Get<int8_t>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<int8_t>());
				}
				else if (type == meta::Type::Get<uint8_t>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<uint8_t>());
				}
				else if (type == meta::Type::Get<int16_t>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<int16_t>());
				}
				else if (type == meta::Type::Get<uint16_t>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<uint16_t>());
				}
				else if (type == meta::Type::Get<int32_t>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<int32_t>());
				}
				else if (type == meta::Type::Get<uint32_t>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<uint32_t>());
				}
				else if (type == meta::Type::Get<int64_t>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<int64_t>());
				}
				else if (type == meta::Type::Get<uint64_t>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<uint64_t>());
				}
				else if (type == meta::Type::Get<float>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<float>());
				}
				else if (type == meta::Type::Get<double>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<double>());
				}
				else if (type == meta::Type::Get<std::string>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<std::string>());
				}
				else if (type == meta::Type::Get<glm::mat4>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<glm::mat4>());
				}
				else if (type == meta::Type::Get<glm::mat3>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<glm::mat3>());
				}
				else if (type == meta::Type::Get<glm::vec2>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<glm::vec2>());
				}
				else if (type == meta::Type::Get<glm::vec3>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<glm::vec3>());
				}
				else if (type == meta::Type::Get<glm::vec4>())
				{
					field.SetValue(instance, data[field.GetName().data()].as<glm::vec4>());
				}
				else if (type.IsEnum())
				{
					std::string enum_name = data[field.GetName().data()].as<std::string>();
					for (auto& enum_value : type.GetEnumValues())
					{
						if (enum_value.GetName() == enum_name)
						{
							field.SetValue(instance, enum_value.GetValue());
							break;
						}
					}
				}
				else if (type.IsArray())
				{
					//todo any push element
					LIGHT_ASSERT(false, "todo any push element");
					//field.SetValue(instance, data[field.GetName().data()].as<bool>());
				}
				else
				{
					meta::Any value = field.GetRefValue(instance);
					DeserializeAny(data[field.GetName().data()], value);
				}
			}
		}		
	}

	SceneSerializer::SceneSerializer(Scene* scene)
		: scene_(scene)
	{
		
	}

	void SceneSerializer::SerializeText(const std::string& filepath)
	{
		YAML::Emitter out;
		
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		std::vector<meta::Type> types = meta::Registry::Get().FindSubTypes<Component>();
		scene_->Each([&types, &out](Entity entity) {
			out << YAML::BeginMap << YAML::Key << "Entity" << YAML::Value << (uint32_t)entity;
			out << YAML::Key << "Components" << YAML::BeginSeq;
			for (auto& type : types)
			{
				if (type.HasComponent(entity))
				{
					auto component = type.GetComponent(entity);
					SerializeAny(out, component);
				}
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;
			});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool SceneSerializer::DeserializeText(const std::string& filepath)
	{
		std::ifstream fin(filepath);
		std::stringstream sstream;
		sstream << fin.rdbuf();

		YAML::Node data = YAML::Load(sstream.str());

		if(!data["Scene"])
			return false;

		std::string scene_name = data["Scene"].as<std::string>();

		auto entities = data["Entities"];

		if (entities)
		{
			for (auto entity : entities)
			{
				Entity deserialize_entity = scene_->CreateEmptyEntity();

				uint32_t id = entity["Entity"].as<uint32_t>();

				auto components = entity["Components"];
				for (auto component : components)
				{
					std::string type_name = component["Type"].as<std::string>();
					meta::Type type(type_name);

					LIGHT_ASSERT(type.IsValid(), "Invalid type!");

					meta::Any deserialize_component = type.AddComponent(deserialize_entity);
					// 空类型component返回的是void
					if (deserialize_component.IsValid()) 
					{
						DeserializeAny(component, deserialize_component);
					}
				}
			}
		}

		return true;
	}
}