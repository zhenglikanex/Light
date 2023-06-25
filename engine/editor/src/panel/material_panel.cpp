#include "material_panel.h"

#include "engine/asset/asset_manager.h"
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
					ImGui::Text("Material Parameter");

					for (auto& [name, decl] : material_->GetShader()->GetParamDeclarations())
					{
						if (decl.type == "float")
						{
							float value = material_->Get<float>(name);
							if (ImGui::DragFloat(name.c_str(), &value, 0.01f))
							{
								material_->Set(name, value);
							}
						}
						else if (decl.type == "float2")
						{
							glm::vec2 value = material_->Get<glm::vec2>(name);
							if (ImGui::DragFloat2(name.c_str(), glm::value_ptr(value), 0.01f))
							{
								material_->Set(name, value);
							}
						}
						else if (decl.type == "float3")
						{
							glm::vec3 value = material_->Get<glm::vec3>(name);

							if (name.find("color") != std::string::npos
								|| name.find("Color") != std::string::npos)
							{
								if (ImGui::ColorEdit3(name.c_str(), glm::value_ptr(value), 0.01f))
								{
									material_->Set(name, value);
								}
							}
							else
							{
								if (ImGui::DragFloat3(name.c_str(), glm::value_ptr(value), 0.01f))
								{
									material_->Set(name, value);
								}
							}
						}
						else if (decl.type == "float4")
						{
							glm::vec4 value = material_->Get<glm::vec4>(name);

							if (name.find("color") != std::string::npos
								|| name.find("Color") != std::string::npos)
							{
								if (ImGui::ColorEdit4(name.c_str(), glm::value_ptr(value), 0.01f))
								{
									material_->Set(name, value);
								}
							}
							else
							{
								if (ImGui::DragFloat3(name.c_str(), glm::value_ptr(value), 0.01f))
								{
									material_->Set(name, value);
								}
							}
						}
						else if (decl.type == "int")
						{
							int value = material_->Get<int>(name);
							if (ImGui::DragInt(name.c_str(), &value))
							{
								material_->Set(name, value);
							}
						}
						else if (decl.type == "int2")
						{
							glm::ivec2 value = material_->Get<glm::ivec2>(name);
							if (ImGui::DragInt2(name.c_str(), glm::value_ptr(value)))
							{
								material_->Set(name, value);
							}
						}
						else if (decl.type == "int3")
						{
							glm::ivec3 value = material_->Get<glm::ivec3>(name);
							if (ImGui::DragInt3(name.c_str(), glm::value_ptr(value)))
							{
								material_->Set(name, value);
							}
						}
						else if (decl.type == "int4")
						{
							glm::ivec4 value = material_->Get<glm::ivec4>(name);
							if (ImGui::DragInt4(name.c_str(), glm::value_ptr(value)))
							{
								material_->Set(name, value);
							}
						}
						else
						{

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