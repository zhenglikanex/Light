#include "engine/serializer/cube_map_serializer.h"

#include <fstream>

namespace light
{
	CubeMapSerializer::CubeMapSerializer(CubeMap* cubemap)
		: cubemap_(cubemap)
	{
		
	}

	void CubeMapSerializer::SerializeText(const std::string& filepath)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;
		out << YAML::Key << "CubeMap" << YAML::Value << filepath;
		out << YAML::Key << "Faces" << YAML::BeginSeq;

		for (int i = 0; i < 6; ++i)
		{
			out << YAML::Value << uuid::ToString(cubemap_->faces[i]);
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool CubeMapSerializer::DeserializeText(const std::string& filepath)
	{
		std::ifstream fin(filepath);

		std::stringstream sstream;
		sstream << fin.rdbuf();

		YAML::Node data = YAML::Load(sstream.str());
		if (!data["CubeMap"])
		{
			return false;
		}

		YAML::Node faces = data["Faces"];
		for (int i = 0; i < 6; ++i)
		{
			auto result = uuid::FromString(faces[i].as<std::string>());
			if (result)
			{
				cubemap_->faces[i] = result.value();
			}
		}

		return true;
	}
}