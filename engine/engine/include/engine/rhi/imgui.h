#pragma once

#include "engine/rhi/device.h"

namespace light::rhi
{
	class Imgui
	{
	public:
		virtual ~Imgui() = default;

		virtual bool Init(Device* device) = 0;

		virtual void BeginFrame() = 0;

		virtual void OnRender(const RenderTarget& render_target) = 0;

		virtual void Shutdown() = 0;
	};

	extern Imgui* CreateImgui();
}