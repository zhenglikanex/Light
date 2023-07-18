#include "cubemap_panel.h"

#include "engine/serializer/cube_map_serializer.h"
#include "engine/editor/imgui_utils.h"
#include "engine/asset/asset_manager.h"
#include "engine/core/application.h"

namespace light::editor
{
	Ref<CubeMap> CubeMapPanel::cubemap_ = nullptr;
	rhi::TextureHandle CubeMapPanel::faces_[6] = {};
	bool CubeMapPanel::open_ = false;

	void CubeMapPanel::ShowCubeMap(CubeMap* cubemap)
	{
		cubemap_ = cubemap;
		open_ = true;
	}

	void CubeMapPanel::OnImguiRender()
	{
		if (!open_)
		{
			return;
		}

		if (ImGui::Begin("CubeMap", &open_))
		{
			if (cubemap_)
			{
				ImGui::Text("Faces");
			
				static const char* face_names[6] = { "Right","Left","Top","Buttom","Front","Back" };

				for (int i = 0; i < 6; ++i)
				{
					ImGui::Text(face_names[i]);
					ImGui::SameLine();

					if (faces_[i])
					{
						auto commnad_list = Application::Get().GetDevice()->GetCommandList(rhi::CommandListType::kDirect);
						commnad_list->TransitionBarrier(faces_[i], rhi::ResourceStates::kPixelShaderResource);
						commnad_list->ExecuteCommandList();

						ImGui::SetCursorPosX(80);
						ImGui::Image(faces_[i]->GetTextureID(), ImVec2(200, 200));
						if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight))
						{
							if (ImGui::MenuItem("Reset"))
							{
								faces_[i] = nullptr;
							}

							ImGui::EndPopup();
						}
					}
					else
					{
						ImGui::SetCursorPosX(80);
						ImGui::Button(" ", ImVec2(200, 200));
					}

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ImGuiEditor::kAssetMetaDragId))
						{
							size_t* meta_address = (size_t*)payload->Data;
							AssetMeta* meta = (AssetMeta*)*meta_address;

							if (meta->IsVaild() && AssetType::kTexture == meta->type)
							{
								auto texture = CastRef<rhi::Texture>(AssetManager::LoadAsset(*meta));
								faces_[i] = texture;

								cubemap_->faces[i] = faces_[i]->uuid;
							}
						}
						ImGui::EndDragDropTarget();
					}
				}

				if (ImGui::Button("Save"))
				{
					CubeMapSerializer cs(cubemap_);
					cs.SerializeText(AssetManager::GetAssetAbsolutePath(cubemap_->GetFilePath()).generic_string());
				}
			}

			ImGui::End();
		}
	}
}

