#include "d12_imgui_renderer.h"
#include "imgui_impl_dx12.h"
#include "d12_device.h"

namespace light::rhi
{
	bool D12ImGuiRenderer::Init(Device* device)
	{
		d12_device_ = CheckedCast<D12Device*>(device);

        auto* d3d_device = d12_device_->GetNative();

        int num_descriptors = 1024;

		{
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.NumDescriptors = num_descriptors;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (d3d_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pd3d_srv_desc_heap_)) != S_OK)
                return false;
        }

        for (UINT i = 0; i < kNumFramesInFlight; i++)
            if (d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frame_context_[i].command_allocator)) != S_OK)
                return false;

        if (d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frame_context_[0].command_allocator, nullptr, IID_PPV_ARGS(&pd3d_command_list_)) != S_OK ||
            pd3d_command_list_->Close() != S_OK)
            return false;

        if (d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)) != S_OK)
            return false;

        fence_event_ = CreateEvent(NULL, FALSE, FALSE, nullptr);
        if (fence_event_ == NULL)
            return false;

		ImGui_ImplDX12_Init(d12_device_->GetNative(), kNumFramesInFlight, DXGI_FORMAT_R8G8B8A8_UNORM,
            pd3d_srv_desc_heap_,
            pd3d_srv_desc_heap_->GetCPUDescriptorHandleForHeapStart(),
            pd3d_srv_desc_heap_->GetGPUDescriptorHandleForHeapStart(), num_descriptors);

        return true;
	}

    void D12ImGuiRenderer::BeginFrame()
    {
        ImGui_ImplDX12_NewFrame();
        ::ImGui::NewFrame();
    }

	void D12ImGuiRenderer::EndFrame(const RenderTarget& render_target)
	{
        // Rendering
        ImGui::Render();

        auto draw_data = ::ImGui::GetDrawData();
        if (draw_data && draw_data->CmdListsCount != 0)
        {
            FrameContext* frameCtx = WaitForNextFrameResources();
            frameCtx->command_allocator->Reset();

            pd3d_command_list_->Reset(frameCtx->command_allocator, nullptr);

            auto d12_texture = CheckedCast<D12Texture*>(render_target.GetAttachment(AttachmentPoint::kColor0).texture.Get());
            auto rtv = d12_texture->GetRTV();

            pd3d_command_list_->OMSetRenderTargets(1, &rtv, false, nullptr);
            pd3d_command_list_->SetDescriptorHeaps(1, &pd3d_srv_desc_heap_);
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pd3d_command_list_);

            pd3d_command_list_->Close();

            auto command_queue = CheckedCast<D12CommandQueue*>(d12_device_->GetCommandQueue(CommandListType::kDirect));
            auto d3d12_queue = command_queue->GetNative();
            d3d12_queue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&pd3d_command_list_));

            ImGuiIO& io = ImGui::GetIO();
            // Update and Render additional Platform Windows
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault(nullptr, (void*)pd3d_command_list_);
            }

            UINT64 fence_value = fence_last_signaled_value_ + 1;
            d3d12_queue->Signal(fence_, fence_value);
            fence_last_signaled_value_ = fence_value;
            frameCtx->fence_value = fence_value;
        }
        else
        {
            ImGuiIO& io = ImGui::GetIO();
            // Update and Render additional Platform Windows
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault(nullptr, (void*)pd3d_command_list_);
            }
        }   
	}

    void D12ImGuiRenderer::Flush()
    {
        WaitForLastSubmittedFrame();
    }

	void D12ImGuiRenderer::Shutdown()
	{
        WaitForLastSubmittedFrame();
		ImGui_ImplDX12_Shutdown();
	}

    ImGuiRenderer* CreateImGuiRenderer()
    {
        return new D12ImGuiRenderer();
    }
}

