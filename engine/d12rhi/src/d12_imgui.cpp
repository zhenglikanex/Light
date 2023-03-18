#include "d12_imgui.h"
#include "imgui_impl_dx12.h"
#include "d12_device.h"

namespace light::rhi
{
	bool D12Imgui::Init(Device* device)
	{
		d12_device_ = CheckedCast<D12Device*>(device);

        auto* d3d_device = d12_device_->GetNative();

        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.NumDescriptors = 1;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (d3d_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3d_srv_desc_heap_)) != S_OK)
                return false;
        }

        {
            D3D12_COMMAND_QUEUE_DESC desc = {};
            desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            desc.NodeMask = 1;
            if (d3d_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3d_command_queue_)) != S_OK)
                return false;
        }

        for (UINT i = 0; i < kNumFramesInFlight; i++)
            if (d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].command_allocator)) != S_OK)
                return false;

        if (d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].command_allocator, nullptr, IID_PPV_ARGS(&g_pd3d_command_list_)) != S_OK ||
            g_pd3d_command_list_->Close() != S_OK)
            return false;

        if (d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence_)) != S_OK)
            return false;

        g_fence_event_ = CreateEvent(NULL, FALSE, FALSE, nullptr);
        if (g_fence_event_ == NULL)
            return false;

		ImGui_ImplDX12_Init(d12_device_->GetNative(), kNumFramesInFlight, DXGI_FORMAT_R8G8B8A8_UNORM,
            g_pd3d_srv_desc_heap_,
            g_pd3d_srv_desc_heap_->GetCPUDescriptorHandleForHeapStart(),
            g_pd3d_srv_desc_heap_->GetGPUDescriptorHandleForHeapStart());

        return true;
	}

    void D12Imgui::BeginFrame()
    {
        ImGui_ImplDX12_NewFrame();
        ImGui::NewFrame();
    }

	void D12Imgui::OnRender()
	{
        // Rendering
        ImGui::Render();

        FrameContext* frameCtx = WaitForNextFrameResources();
        frameCtx->command_allocator->Reset();

        g_pd3d_command_list_->Reset(frameCtx->command_allocator, nullptr);

        g_pd3d_command_list_->SetDescriptorHeaps(1, &g_pd3d_srv_desc_heap_);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3d_command_list_);
        g_pd3d_command_list_->Close();

        g_pd3d_command_queue_->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&g_pd3d_command_list_));

        ImGuiIO& io = ImGui::GetIO();
        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault(nullptr, (void*)g_pd3d_command_list_);
        }

        UINT64 fence_value = g_fence_last_signaled_value_ + 1;
        g_pd3d_command_queue_->Signal(g_fence_, fence_value);
        g_fence_last_signaled_value_ = fence_value;
        frameCtx->fence_value = fence_value;
	}

	void D12Imgui::Shutdown()
	{
		ImGui_ImplDX12_Shutdown();
	}

    Imgui* CreateImgui()
    {
        return new D12Imgui();
    }
}

