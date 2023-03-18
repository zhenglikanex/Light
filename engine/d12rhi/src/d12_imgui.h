#pragma once

#include "engine/rhi/imgui.h"

#include "imgui_impl_dx12.h"

#include <d3d12.h>
#include <dxgi1_4.h>

namespace light::rhi
{
	class D12Device;

	class D12Imgui : public Imgui
	{
	public:
		// 通过 Imgui 继承
		bool Init(Device* device) override;

		void BeginFrame() override;

		void OnRender() override;

		void Shutdown() override;

	private:
		struct FrameContext
		{
			ID3D12CommandAllocator* command_allocator;
			UINT64 fence_value;
		};

		void WaitForLastSubmittedFrame()
		{
			FrameContext* frame_ctx = &g_frameContext[g_frameIndex % kNumFramesInFlight];

			UINT64 fence_value = frame_ctx->fence_value;
			if (fence_value == 0)
				return; // No fence was signaled

			frame_ctx->fence_value = 0;
			if (g_fence_->GetCompletedValue() >= fence_value)
				return;

			g_fence_->SetEventOnCompletion(fence_value, g_fence_event_);
			WaitForSingleObject(g_fence_event_, INFINITE);
		}

		FrameContext* WaitForNextFrameResources()
		{
			UINT next_frame_index = g_frameIndex + 1;
			g_frameIndex = next_frame_index;

			FrameContext* frame_ctx = &g_frameContext[next_frame_index % kNumFramesInFlight];
			if (UINT64 fence_value = frame_ctx->fence_value; fence_value != 0) // means no fence was signaled
			{
				frame_ctx->fence_value = 0;
				g_fence_->SetEventOnCompletion(fence_value, g_fence_event_);

				WaitForSingleObject(g_fence_event_, INFINITE);
			}

			return frame_ctx;
		}

		// Data
		static constexpr int kNumFramesInFlight = 3;

		D12Device* d12_device_ = nullptr;
		FrameContext g_frameContext[kNumFramesInFlight] = {};
		UINT g_frameIndex = 0;
		ID3D12DescriptorHeap* g_pd3d_rtv_desc_heap_ = nullptr;
		ID3D12DescriptorHeap* g_pd3d_srv_desc_heap_ = nullptr;
		ID3D12CommandQueue* g_pd3d_command_queue_ = nullptr;
		ID3D12GraphicsCommandList* g_pd3d_command_list_ = nullptr;
		ID3D12Fence* g_fence_ = nullptr;
		HANDLE g_fence_event_ = nullptr;
		UINT64 g_fence_last_signaled_value_ = 0;
	};
}

