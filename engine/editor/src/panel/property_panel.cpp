#include "property_panel.h"
#include "engine/scene/components.h"
#include "engine/reflection/attribute.h"
#include "imgui_internal.h"

namespace light::editor
{
	void PropertyPanel::OnImguiRender()
	{
		ImGui::Begin("PropertyPanel");

		if(select_entity_)
		{
			{
				auto& comp = select_entity_.GetComponent<TagComponent>();
				DrawComponentProperty(std::ref(comp));
			}
			
			{
				if(select_entity_.HasComponent<SpriteRendererComponent>())
				{
					auto& comp = select_entity_.GetComponent<SpriteRendererComponent>();

					DrawComponentProperty(std::ref(comp));
				}
			}

			{
				if (select_entity_.HasComponent<TransformComponent>())
				{
					auto& comp = select_entity_.GetComponent<TransformComponent>();

					DrawComponentProperty(std::ref(comp));
				}
			}

			{
				if (select_entity_.HasComponent<CameraComponent>())
				{
					auto& comp = select_entity_.GetComponent<CameraComponent>();

					DrawComponentProperty(std::ref(comp));
				}
			}

			if(ImGui::Button("AddComponent"))
			{
				ImGui::OpenPopup("AddComponent");
			}

			if(ImGui::BeginPopup("AddComponent"))
			{
				if(ImGui::MenuItem("TransformComponent"))
				{
					//if(select_entity_.HasComponent<Tra>())
				}
				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}

	void PropertyPanel::OnEvent(const Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<SelectEntityEvent>(BIND_EVENT_1(&PropertyPanel::OnSelectEntityEvent, this));
	}

	void PropertyPanel::OnSelectEntityEvent(const SelectEntityEvent& e)
	{
		select_entity_ = e.entity;
	}

	void PropertyPanel::DrawComponentProperty(meta::Any instance)
	{
		std::string name = instance.GetType().GetName();
		std::string_view name_ref = name;
		if (name_ref.find_last_of(':') != std::string_view::npos)
		{
			name_ref = name_ref.substr(name_ref.find_last_of(':') + 1);
		}

		bool open_tree = ImGui::TreeNodeEx(instance.GetType().GetName().data(), ImGuiTreeNodeFlags_OpenOnArrow, name_ref.data());
		if(open_tree)
		{
			DrawTypeProperty(instance);
			ImGui::TreePop();
		}
	}

	static void DrawVec3Control(std::string_view label, glm::vec3& values, float reset_value = 0.0f, float column_width = 100.0f)
	{
		ImGui::PushID(label.data());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, column_width);
		ImGui::Text(label.data());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 button_size = { line_height + 3.0f, line_height };
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		
		if (ImGui::Button("X", button_size))
		{
			values.x = reset_value;
		}

		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &values.x, 0.1f))

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });

		if (ImGui::Button("Y", button_size))
		{
			values.y = reset_value;
		}

		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &values.y, 0.1f))

		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.3f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.2f, 0.8f, 1.0f });

		if (ImGui::Button("Z", button_size))
		{
			values.z = reset_value;
		}

		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &values.z, 0.1f))

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	void PropertyPanel::DrawTypeProperty(meta::Any& instance)
	{
		for (auto& field : instance.GetType().GetFields())
		{
			auto condition = field.GetProperty<meta::attribute::EditorCondition>();
			if(condition)
			{
				if(instance.GetType().GetField(condition->field_name).GetValue(instance).Cast<uint64_t>() != condition->value)
				{
					continue;
				}
			}

			auto show_in_editor = field.GetProperty<meta::attribute::ShowInEditor>();
			if(show_in_editor && !show_in_editor->visible)
			{
				continue;
			}

			meta::Type field_type = field.GetType();
			if (field_type == meta::Type::Get<std::string>())
			{
				std::string value = field.GetValue(instance).Cast<std::string>();
				value.resize(256);
				if (ImGui::InputText(field.GetName().data(), value.data(), value.size()))
				{
					SetFieldValue(instance, field, value);
				}
			}
			else if (field_type == meta::Type::Get<float>())
			{
				float value = field.GetValue(instance).Cast<float>();
				if(ImGui::DragFloat(field.GetName().data(), &value, 0.1))
				{
					SetFieldValue(instance, field, value);
				}
			}
			else if (field_type == meta::Type::Get<bool>())
			{
				bool value = field.GetValue(instance).Cast<bool>();
				if(ImGui::Checkbox(field.GetName().data(),&value))
				{
					SetFieldValue(instance, field, value);
				}
			}
			else if (field_type == meta::Type::Get<glm::vec4>())
			{
				glm::vec4 value = field.GetValue(instance).Cast<glm::vec4>();
				if(ImGui::ColorEdit4(field.GetName().data(), glm::value_ptr(value)))
				{
					SetFieldValue(instance, field, value);
				}
			}
			else if (field_type == meta::Type::Get<glm::vec3>())
			{
				glm::vec3& value = field.GetRefValue(instance).Cast<glm::vec3>();
				DrawVec3Control(field.GetName(), value);
			}
			else if (field_type.IsEnum())
			{
				auto& enum_values = field_type.GetEnumValues();
				std::vector<const char*> items;
				items.reserve(enum_values.size());

				int64_t current_value = field.GetValue(instance).Cast<int64_t>();
				int64_t current_index = 0;
				for (size_t index = 0; index < enum_values.size(); ++index)
				{
					auto& enum_value = enum_values[index];

					items.emplace_back(enum_value.GetName().data());
					if (enum_value.GetValue() == current_value)
					{
						current_index = index;
					}
				}

				static int item_current_idx = 0; // Here we store our selection data as an index.
				if (ImGui::BeginCombo(field.GetName().data(), items[current_index], ImGuiComboFlags_None))
				{
					for (int n = 0; n < items.size(); n++)
					{
						const bool is_selected = (item_current_idx == n);
						if (ImGui::Selectable(items[n], is_selected))
						{
							item_current_idx = n;
							SetFieldValue(instance, field, enum_values[n].GetValue());
						}

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
			else if (field.GetType() == meta::Type::Get<glm::mat4>())
			{
				// not doing
			}
			else
			{
				meta::Any value = field.GetRefValue(instance);
				bool open_tree = ImGui::TreeNodeEx(value.Cast<void*>(), ImGuiTreeNodeFlags_OpenOnArrow, field.GetName().data());
				if (open_tree)
				{
					DrawTypeProperty(value);
					ImGui::TreePop();
				}
			}
		}
	}
}

