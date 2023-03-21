#pragma once

#include "engine/event/event.h"
#include "engine/rhi/render_target.h"

namespace light
{
	// 用层来表达不同模块之间的渲染顺序和覆盖关系
	// 影响模块之间的渲染顺序和事件接受
	// 比如UI层和3D层的覆盖关系和渲染顺序
	class Layer
	{
	public:
		virtual ~Layer() {};

		virtual void OnAttach() {};
		virtual void OnDetach() {};

		virtual void OnUpdate() {};
		virtual void OnImGuiRender() {};

		virtual void OnEvent(const Event& e) {};
	};
}
