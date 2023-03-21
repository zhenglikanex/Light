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
		virtual ~Layer() = default;

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;

		virtual void OnUpdate() = 0;
		virtual void OnImGuiRender() = 0;

		virtual void OnEvent(const Event& e) = 0;
	};
}
