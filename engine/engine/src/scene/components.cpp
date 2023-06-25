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
		}
		
		if (ImGui::TreeNode("Materials"))
		{
			if (mesh)
			{
				for (uint32_t i = 0; i < mesh->GetNumSubMesh(); ++i)
				{
					Material* mat = mesh->GetMaterial(i);
					ImGui::PushID(i);
					ImGui::Text("%d.", i + 1);
					ImGui::SameLine();
					if (Asset* select_asset = ImGuiEditor::InputAsset(AssetType::kMaterial, mat))
					{
						mesh->SetMaterial(i, CheckedCast<Material*>(select_asset));
					}
					ImGui::PopID();
				}

				ImGui::TreePop();
			}
		}
#endif
	}

	void MeshComponent::SerializeText(YAML::Emitter* out)
	{
		*out << YAML::Key << "mesh" << YAML::Value << mesh->GetUuidString();
		*out << YAML::Key << "materials" << YAML::Value << YAML::BeginSeq;
		for (int i = 0; i < mesh->GetNumSubMesh(); ++i)
		{
			Material* mat = mesh->GetMaterial(i);
			*out << mat->GetUuidString();
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

		uint32_t i = 0;
		for (auto mat_node : node["materials"])
		{
			std::optional<UUID> result = uuid::FromString(mat_node.as<std::string>());
			if (!result.has_value())
			{
				LOG_ENGINE_WARN("Entity Material Miss");
				continue;
			}

			Material* material = AssetManager::LoadAsset<Material>(result.value());

			if (!result.has_value())
			{
				LOG_ENGINE_WARN("Entity Material Miss");
				continue;
			}

			mesh->SetMaterial(i++, material);
		}
	}
}