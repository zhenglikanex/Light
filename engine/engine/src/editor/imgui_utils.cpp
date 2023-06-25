#include "engine/editor/imgui_utils.h"
#include "engine/asset/asset.h"
#include "engine/asset/asset_manager.h"

#ifdef  LIGHT_EDITOR

namespace light::ImGuiEditor
{
	Asset* InputAsset(AssetType asset_type,const Asset* asset)
	{
		const char* select = "Select";
		const char* asset_path = asset && !asset->uuid.is_nil() ? asset->GetFilePath().c_str() : " ";

		ImVec2 size = ImGui::GetContentRegionAvail();
		ImGui::Button(asset_path, { size.x - ImGui::CalcTextSize(select).x - ImGui::GetStyle().ItemSpacing.x * 2,0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(kAssetMetaDragId))
			{
				size_t* meta_address = (size_t*)payload->Data;
				AssetMeta* meta = (AssetMeta*)*meta_address;

				if (meta->IsVaild() && asset_type == meta->type)
				{
					return AssetManager::LoadAsset(*meta);
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();

		if (ImGui::Button(select))
		{
			// todo : 编辑器模式下，选择模型
			
		}

		return nullptr;
	}
}

#endif
