#pragma once

#include "engine/renderer/material.h"

namespace light::editor
{
	class MaterialPanel
	{
	public:
		static void ShowMaterial(Material* material);

		MaterialPanel() = default;
	
		void OnImguiRender();
	private:
		static Ref<Material> material_;
		static bool open_;
	};
}