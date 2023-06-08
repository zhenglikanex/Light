#include "material_panel.h"


#include "engine/serializer/material_serializer.h"
#include "engine/utils/platform_utils.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

namespace light::editor
{
	void MaterialPanel::SelectMaterial(Material* material)
	{
		material_ = material;
	}

	void MaterialPanel::OnImguiRender()
	{
		if (material_)
		{
			ImGui::Begin("Material");

			const Shader* shader = material_->GetShader();
			ImGui::LabelText("Shader",shader->GetFilePath().c_str());
			
			ImGui::Text("Material Params");

			if (material_->GetShader()->GetParamDeclarations().size() > 0)
			{
				for (auto& [name, decl] : material_->GetShader()->GetParamDeclarations())
				{
					if (decl.type == "float")
					{
						float value = material_->Get<float>(name);
						if (ImGui::DragFloat(name.c_str(), &value,0.01f))
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
							if (ImGui::DragFloat3(name.c_str(), glm::value_ptr(value),0.01f))
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
				std::string filepath = FileDialogs::SaveFile("pbr.mtl","Light Material (*.mtl)\0*.mtl\0");
				if (!filepath.empty())
				{
					MaterialSerializer serializer(material_);
					serializer.SerializeText(filepath);
				}
			}

			ImGui::End();
		}
	}
}