#pragma once

#include <vector>
#include <memory>

#include "engine/layer/layer.h"
#include "engine/event/event.h"
#include "engine/rhi/render_target.h"

namespace light
{
	class LayerStack
	{
	public:
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlayLayer(Layer* layer);

		void OnEvent(const Event& e);

		void OnUpdate(const rhi::RenderTarget& render_target);
	private:
		std::vector<std::unique_ptr<Layer>> layers_;
		std::vector<std::unique_ptr<Layer>> overlay_layers_;
	};
}
