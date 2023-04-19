#include "scene_hierarchy_panel.h"

namespace light::editor
{
	void SceneHierarchyPanel::OnImguiRender()
	{
		ImGui::Begin("SceneHierarchy");

		scene_->GetRegistry().each([&](auto entity_id)
			{
				Entity entity(entity_id, scene_);
				DrawEntityNode(entity);
			});

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity e)
	{
		ImGuiTreeNodeFlags flags = ((selected_entity_ == (uint32_t)e) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool node_open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)e, flags, e.GetComponent<TagComponent>().tag.c_str());
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			selected_entity_ = (uint32_t)e;
		}
		
		if (node_open)
		{
			LOG_ENGINE_INFO("NODE_OPEN");
			
			bool open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)e, flags, e.GetComponent<TagComponent>().tag.c_str());
			if (open)
			{
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}
}

