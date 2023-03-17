#pragma once

#include "engine/rhi/device.h"

namespace light::rhi
{
	class Imgui
	{
	public:
		virtual void Init(Device* device) = 0;

		virtual void OnUpdate() = 0;

		virtual void Shutdown() = 0;
	};
}