#pragma once

#include "engine/light.h"

namespace light::editor
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;

		void SetScene(Scene* scene);

		void OnImguiRender();

		void DrawEntityNode(Entity e);
	private:
		void SelecteEntity(Entity e);

		Scene* scene_ = nullptr;
		Entity selected_entity_ = {};
	};
}