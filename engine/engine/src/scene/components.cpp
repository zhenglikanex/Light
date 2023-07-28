#include "engine/scene/components.h"

#include "engine/asset/asset_manager.h"

#include "engine/utils/platform_utils.h"
#include "engine/serializer/material_serializer.h"

#include "engine/editor/imgui_utils.h"

#include "yaml-cpp/yaml.h"

namespace light
{
	void MeshComponent::ImGuiDrawProperty()
	{
#ifdef LIGHT_EDITOR
		ImGui::Text("Mesh");
		
		if (Asset* select_asset = ImGuiEditor::InputAsset(AssetType::kMesh, mesh))
		{
			mesh = CheckedCast<Mesh*>(select_asset);
			materials.resize(mesh->GetNumSubMesh());
		}
		
		if (ImGui::TreeNode("Materials"))
		{
			if (mesh)
			{
				for (uint32_t i = 0; i < mesh->GetNumSubMesh(); ++i)
				{
					if (materials.size() <= i)
					{
						materials.resize(i + 1);
					}

					Material* mat = materials[i];
					ImGui::PushID(i);
					ImGui::Text("%d.", i + 1);
					ImGui::SameLine();
					if (Asset* select_asset = ImGuiEditor::InputAsset(AssetType::kMaterial, mat))
					{
						if (materials.size() <= i)
						{
							materials.resize(i + 1);
						}
						materials[i] = CheckedCast<Material*>(select_asset);
					}
					ImGui::PopID();
				}
			}

			ImGui::TreePop();
		}
#endif
	}

	void MeshComponent::SerializeText(YAML::Emitter* out)
	{
		*out << YAML::Key << "mesh" << YAML::Value << (mesh ? mesh->GetUuidString() : "");
		*out << YAML::Key << "materials" << YAML::Value << YAML::BeginSeq;

		for (auto material : materials)
		{
			if (material)
			{
				*out << material->GetUuidString();
			}
			else
			{
				*out << "";
			}
		}

		*out << YAML::EndSeq;
	}

	void MeshComponent::DeserializeText(YAML::Node node)
	{
		std::optional<UUID> result = uuid::FromString(node["mesh"].as<std::string>());
		if (!result.has_value())
		{
			LOG_ENGINE_WARN("Mesh Miss");
			return;
		}

		mesh = AssetManager::LoadAsset<Mesh>(result.value());
		if (!mesh)
		{
			LOG_ENGINE_WARN("Entity Mesh Miss");
			return;
		}

		materials.resize(node["materials"].size());

		uint32_t i = 0;
		for (auto mat_node : node["materials"])
		{
			std::string uuid = mat_node.as<std::string>();
			if (uuid.empty())
			{
				continue;
			}

			std::optional<UUID> result = uuid::FromString(uuid);
			if (!result.has_value())
			{
				LOG_ENGINE_WARN("Entity Material Miss");
				continue;
			}

			Material* material = AssetManager::LoadAsset<Material>(result.value());
			if (!material)
			{
				LOG_ENGINE_WARN("Entity Material Miss");
				continue;
			}

			materials[i++] = material;
		}
	}

	void SkeletalMeshComponent::ComputeGlobalPose()
	{
		
	}
}

