#pragma once

#include "engine/layer/layer.h"
#include "engine/rhi/render_target.h"

namespace light
{
	class ImguiLayer : public Layer
	{
	public:
		// 通过 Layer 继承
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(const rhi::RenderTarget& render_target) override;
		void OnEvent(const Event& e) override;
	private:

		
	};
}
