#pragma once

#include "light_pch.h"

#include "engine/rhi/render_target.h"
#include "engine/rhi/command_list.h"

namespace light
{
	struct RenderPassResources
	{
		rhi::RenderTarget render_target;
	};

	class RenderPass : public RefCounter
	{
	public:
		RenderPass(const RenderPassResources& resources);

		const RenderPassResources& GetResources() const { return resources_; }
	private:
		RenderPassResources resources_;
	};
}