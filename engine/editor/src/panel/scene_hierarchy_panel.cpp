#include "scene_hierarchy_panel.h"
#include "../editor_event.h"

namespace light::editor
{
	void SceneHierarchyPanel::OnImguiRender()
	{
		ImGui::Begin("SceneHierarchy");

		if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				scene_->CreateEntity();
			}

			ImGui::EndPopup();
		}

		scene_->GetRegistry().each([&](auto entity_id)
			{
				Entity entity(entity_id, scene_);
				DrawEntityNode(entity);
			});

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity e)
	{
		ImGuiTreeNodeFlags flags = ((selected_entity_ == e) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool node_open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)e, flags, e.GetComponent<TagComponent>().tag.c_str());
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			selected_entity_ = e;

			SelectEntityEvent event(e);
			Application::Get().OnEvent(event);
		}

		bool delete_entity = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				delete_entity = true;
			}

			ImGui::EndPopup();
		}

		if (node_open)
		{
			bool open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)e, flags, e.GetComponent<TagComponent>().tag.c_str());
			if (open)
			{
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if(delete_entity)
		{
			scene_->DestroyEntity(e);
			if(e == selected_entity_)
			{
				selected_entity_ = {};
			}
		}
	}
}

