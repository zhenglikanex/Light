#include "engine/layer/layer_stack.h"

#include "engine/rhi/render_target.h"

namespace light
{
	LayerStack::~LayerStack()
	{
		{
			auto it = layers_.begin();
			while (it != layers_.end())
			{
				(*it)->OnDetach();
				++it;
			}
		}

		{
			auto it = overlay_layers_.begin();
			while (it != overlay_layers_.end())
			{
				(*it)->OnDetach();
				++it;
			}
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		layers_.emplace_back(layer);
	}

	void LayerStack::PushOverlayLayer(Layer* layer)
	{
		overlay_layers_.emplace_back(layer);
	}

	void LayerStack::OnEvent(const Event& e)
	{
		// ???????????????????,????и??????
		{
			auto it = overlay_layers_.rbegin();
			while (it != overlay_layers_.rend())
			{
				(*it)->OnEvent(e);
				++it;
			}
		}
		
		{
			auto it = layers_.rbegin();
			while (it != layers_.rend())
			{
				(*it)->OnEvent(e);
				++it;
			}
		}
	}

	void LayerStack::OnUpdate()
	{
		{
			auto it = layers_.begin();
			while (it != layers_.end())
			{
				(*it)->OnUpdate();
				++it;
			}
		}

		{
			auto it = overlay_layers_.begin();
			while (it != overlay_layers_.end())
			{
				(*it)->OnUpdate();
				++it;
			}
		}

		{
			auto it = layers_.begin();
			while (it != layers_.end())
			{
				(*it)->OnImGuiRender();
				++it;
			}
		}

		{
			auto it = overlay_layers_.begin();
			while (it != overlay_layers_.end())
			{
				(*it)->OnImGuiRender();
				++it;
			}
		}
	}
}
