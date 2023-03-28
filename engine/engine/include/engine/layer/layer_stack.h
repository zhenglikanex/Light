#pragma once

#include <vector>
#include <memory>

#include "engine/core/timestep.h"

#include "engine/layer/layer.h"
#include "engine/event/event.h"

namespace light
{
	class LayerStack
	{
	public:
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlayLayer(Layer* layer);

		void OnEvent(const Event& e);

		void OnUpdate(const Timestep& ts,bool minimized);

	private:
		std::vector<std::unique_ptr<Layer>> layers_;
		std::vector<std::unique_ptr<Layer>> overlay_layers_;
	};
}
