#include "engine/layer/imgui_layer.h"

#include "imgui.h"

namespace light
{
	void ImguiLayer::OnAttach()
	{

	}

	void ImguiLayer::OnDetach()
	{
	}

	void ImguiLayer::OnUpdate(const Timestep& ts)
	{
		
	}

	void ImguiLayer::OnImGuiRender(const Timestep& ts)
	{

	}

	void ImguiLayer::OnEvent(Event& e)
	{
		if (block_event_)
		{
			ImGuiIO& io = ImGui::GetIO();
			if(io.WantCaptureMouse)
				e.handle |= e.IsCategory(EventCategory::kMouse);
			if(io.WantCaptureKeyboard)
				e.handle |= e.IsCategory(EventCategory::kKey);
			//e.handle |= e.IsCategory(EventCategory::kMouse) && io.WantCaptureMouse;
			//e.handle |= e.IsCategory(EventCategory::kKey) && io.WantCaptureKeyboard;
		}
	}
}

