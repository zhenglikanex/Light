#pragma once

#include "engine/layer/layer.h"
#include "engine/rhi/render_target.h"

namespace light
{
	class ImguiLayer : public Layer
	{
	public:
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(const Timestep& ts) override;
		void OnImGuiRender(const Timestep& ts) override;
		void OnEvent(const Event& e) override;
	private:		
	};
}
