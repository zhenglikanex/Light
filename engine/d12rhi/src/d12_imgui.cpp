#include "d12_imgui.h"
#include "imgui_impl_dx12.h"
#include "d12_device.h"

namespace light::rhi
{
	void D12Imgui::Init(Device* device)
	{
		d12_device_ = CheckedCast<D12Device*>(device);
		ImGui_ImplDX12_Init(d12_device_->GetNative(), 3, DXGI_FORMAT_R8G8B8A8_UNORM, nullptr, {}, {});
	}

	void D12Imgui::OnUpdate()
	{
		auto d12_command_list = CheckedCast<D12CommandList*>(d12_device_->GetCommandList(CommandListType::kDirect));

		ImGui_ImplDX12_NewFrame();

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d12_command_list->GetD3D12GraphicsCommandList());
	}

	void D12Imgui::Shutdown()
	{
		ImGui_ImplDX12_Shutdown();
	}
}

