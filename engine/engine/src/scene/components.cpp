#include "engine/scene/components.h"

#include "engine/utils/platform_utils.h"
#include "engine/serializer/material_serializer.h"

#ifdef LIGHT_EDITOR
#include "imgui.h"
#endif

#include "yaml-cpp/yaml.h"

namespace light
{
	void MeshComponent::ImGuiDrawProperty()
	{
#ifdef LIGHT_EDITOR
		ImGui::Text("Mesh");
		std::string mesh_file = mesh != nullptr ? mesh->GetFileName() : "null";
		ImGui::Text(mesh_file.c_str());
		ImGui::SameLine();
		if (ImGui::Button("Select"))
		{
			std::string file = FileDialogs::OpenFile("Mesh\0 *.obj;*.fbx\0");
			if (!file.empty())
			{
				mesh = MakeRef<Mesh>(file);
			}
		}

		if (ImGui::TreeNode("Materials"))
		{
			if (mesh)
			{
				for (uint32_t i = 0; i < mesh->GetNumSubMesh(); ++i)
				{
					Material* mat = mesh->GetMaterial(i);
					std::string label = mat != nullptr ? std::format("{}.{}", i, mat->GetFilePath()) : std::format("{}.null", i);
					ImGui::Text(label.c_str());
					ImGui::SameLine();
					if (ImGui::Button("Select"))
					{
						std::string file = FileDialogs::OpenFile("Material\0 *.mtl\0");
						if (!file.empty())
						{
							Ref<Material> material = MakeRef<Material>();
							MaterialSerializer serializer(material);
							serializer.DeserializeText(file);

							mesh->SetMaterial(i, material);
						}
					}
				}
				ImGui::TreePop();
			}
		}
#endif
	}

	void MeshComponent::SerializeText(YAML::Emitter* out)
	{
		*out << YAML::Key << "mesh" << YAML::Value << mesh->GetFileName();
		*out << YAML::Key << "materials" << YAML::Value << YAML::BeginSeq;
		for (int i = 0; i < mesh->GetNumSubMesh(); ++i)
		{
			Material* mat = mesh->GetMaterial(i);
			*out << mat->GetFilePath();
		}
		*out << YAML::EndSeq;
	}

	void MeshComponent::DeserializeText(YAML::Node node)
	{
		std::string mesh_file = node["mesh"].as<std::string>();
		mesh = MakeRef<Mesh>(mesh_file);

		uint32_t i = 0;
		for (auto mat_node : node["materials"])
		{
			Ref<Material> material = MakeRef<Material>();
			std::string mat_file = mat_node.as<std::string>();
			MaterialSerializer serializer(material);
			serializer.DeserializeText(mat_file);

			mesh->SetMaterial(i++, material);
		}
	}
}