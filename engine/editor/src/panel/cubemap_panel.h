#pragma once

#include "engine/renderer/cube_map.h"
#include "engine/rhi/texture.h"

namespace light::editor
{
	class CubeMapPanel
	{
	public:
		static void ShowCubeMap(CubeMap* cubemap);

		CubeMapPanel() = default;

		void OnImguiRender();
	private:
		static Ref<CubeMap> cubemap_;
		static rhi::TextureHandle faces_[6];
		static bool open_;
	};
}