#pragma once

#include "engine/light.h"

namespace light::editor
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;

		void SetScene(Scene* scene) { scene_ = scene; }

		void OnImguiRender();

		void DrawEntityNode(Entity e);
	private:
		Scene* scene_ = nullptr;
		Entity selected_entity_ = {};
	};
}