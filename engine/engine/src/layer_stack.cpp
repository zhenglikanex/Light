#include "engine/layer/layer_stack.h"

namespace light
{
	LayerStack::~LayerStack()
	{
		{
			auto it = layers_.begin();
			while (it != layers_.end())
			{
				(*it)->OnDeattach();
			}
		}

		{
			auto it = overlay_layers_.begin();
			while (it != overlay_layers_.end())
			{
				(*it)->OnDeattach();
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
		// ???????????????????,????§Ú??????
		{
			auto it = overlay_layers_.rbegin();
			while (it != overlay_layers_.rend())
			{
				(*it)->OnEvent(e);
			}
		}
		
		{
			auto it = layers_.rbegin();
			while (it != layers_.rend())
			{
				(*it)->OnEvent(e);
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
			}
		}

		{
			auto it = overlay_layers_.begin();
			while (it != overlay_layers_.end())
			{
				(*it)->OnUpdate();
			}
		}
	}
}