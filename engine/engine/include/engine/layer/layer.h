#pragma once

#include "engine/core/timestep.h"

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

		virtual void OnUpdate(const Timestep& ts) {};
		virtual void OnImGuiRender(const Timestep& ts) {};

		virtual void OnEvent(Event& e) { };
	};
}
