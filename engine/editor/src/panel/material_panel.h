#pragma once

#include "engine/renderer/material.h"

namespace light::editor
{
	class MaterialPanel
	{
	public:
		MaterialPanel() = default;

		void SelectMaterial(Material* material);

		void OnImguiRender();
	private:
		Ref<Material> material_;
	};
}