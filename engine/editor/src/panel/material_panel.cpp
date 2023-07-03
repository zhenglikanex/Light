#include "material_panel.h"

#include "engine/core/application.h"
#include "engine/asset/asset_manager.h"
#include "engine/renderer/renderer.h"
#include "engine/serializer/material_serializer.h"
#include "engine/utils/platform_utils.h"
#include "engine/editor/imgui_utils.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace light::editor
{
	Ref<Material> MaterialPanel::material_ = nullptr;
	bool MaterialPanel::open_ = false;

	void MaterialPanel::ShowMaterial(Material* material)
	{
		material_ = material;
		open_ = true;
	}

	void MaterialPanel::OnImguiRender()
	{
		if (!open_)
		{
			return;
		}

		if (ImGui::Begin("Material",&open_))
		{
			if (material_)
			{
				ImGui::Text("Shader");

				ImGui::SameLine();

				if (Asset* input_asset = ImGuiEditor::InputAsset(AssetType::kShader, material_->GetShader()))
				{
					material_->SetShader(CheckedCast<Shader*>(input_asset));
				}

				if (material_->GetShader() && material_->GetShader()->GetParamDeclarations().size() > 0)
				{
					ImGui::Text("Material Properties");

					for (auto& property : material_->GetShader()->GetProperties())
					{
						if (property.type == ShaderPropertyType::kNumber)
						{
							float value = material_->Get<float>(property.variable_name);
							if (ImGui::DragFloat(property.editor_name.c_str(), &value, 0.01f,property.range.min,property.range.max))
							{
								material_->Set(property.variable_name, value);
							}
						}
						else if (property.type == ShaderPropertyType::kColor)
						{
							glm::vec3 value = material_->Get<glm::vec3>(property.variable_name);
							if (ImGui::ColorEdit3(property.editor_name.c_str(), glm::value_ptr(value)))
							{
								material_->Set(property.variable_name, value);
							}
						}
						else if (property.type == ShaderPropertyType::kTexture2D)
						{
							rhi::Texture* texture = material_->Get(property.variable_name);
							if (texture)
							{
								auto commnad_list = Application::Get().GetDevice()->GetCommandList(rhi::CommandListType::kDirect);
								commnad_list->TransitionBarrier(texture, rhi::ResourceStates::kPixelShaderResource);
								commnad_list->ExecuteCommandList();

								ImGui::Text(property.editor_name.c_str());
								ImGui::SameLine();
								ImGui::Image(texture->GetTextureID(), ImVec2(50, 50));
								if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight))
								{
									if (ImGui::MenuItem("Reset"))
									{
										material_->Set(property.variable_name, Renderer::GetBuiltinTexture(property.texture));
									}

									ImGui::EndPopup();
								}
							}
							else
							{
								ImGui::Text(property.editor_name.c_str());
								ImGui::SameLine();
								ImGui::Button(" ", ImVec2(50, 50));
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
										material_->Set(property.variable_name, texture);
									}
								}
								ImGui::EndDragDropTarget();
							}
						}
					}
				}

				if (ImGui::Button("Save"))
				{
					MaterialSerializer ms(material_);
					ms.SerializeText(AssetManager::GetAssetAbsolutePath(material_->GetFilePath()).generic_string());
				}
			}

			ImGui::End();
		}
	}
}