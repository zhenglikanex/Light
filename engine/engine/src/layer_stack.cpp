#include "engine/layer/layer_stack.h"

#include "engine/log/log.h"

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
		layer->OnAttach();
	}

	void LayerStack::PushOverlayLayer(Layer* layer)
	{
		overlay_layers_.emplace_back(layer);
		layer->OnAttach();
	}

	void LayerStack::OnEvent(Event& e)
	{
		{
			auto it = overlay_layers_.rbegin();
			while (it != overlay_layers_.rend())
			{
				if (e.handle)
				{
					return;
				}
				(*it)->OnEvent(e);
				++it;
			}
		}
		
		{
			auto it = layers_.rbegin();
			while (it != layers_.rend())
			{
				if (e.handle)
				{
					return;
				}
				(*it)->OnEvent(e);
				++it;
			}
		}
	}

	void LayerStack::OnUpdate(const Timestep& ts, bool minimized)
	{
		if (!minimized)
		{
			{
				auto it = layers_.begin();
				while (it != layers_.end())
				{
					(*it)->OnUpdate(ts);
					++it;
				}
			}

			{
				auto it = overlay_layers_.begin();
				while (it != overlay_layers_.end())
				{
					(*it)->OnUpdate(ts);
					++it;
				}
			}
		}
		
		{
			auto it = layers_.begin();
			while (it != layers_.end())
			{
				(*it)->OnImGuiRender(ts);
				++it;
			}
		}

		{
			auto it = overlay_layers_.begin();
			while (it != overlay_layers_.end())
			{
				(*it)->OnImGuiRender(ts);
				++it;
			}
		}
	}
}
