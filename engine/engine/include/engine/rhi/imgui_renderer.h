#pragma once

#include "engine/rhi/device.h"

namespace light::rhi
{
	class ImGuiRenderer
	{
	public:
		virtual ~ImGuiRenderer() = default;

		virtual bool Init(Device* device) = 0;

		virtual void BeginFrame() = 0;

		virtual void EndFrame(const RenderTarget& render_target) = 0;

		virtual void Shutdown() = 0;
	};

	extern ImGuiRenderer* CreateImGuiRenderer();
}