#pragma once

#include "engine/layer/layer.h"

namespace light
{
	class ImguiLayer : public Layer
	{
	public:
		// 通过 Layer 继承
		virtual void OnAttach() override;
		virtual void OnDeattach() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(const Event& e) override;
	private:

		
	};
}