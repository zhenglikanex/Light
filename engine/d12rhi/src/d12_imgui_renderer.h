#pragma once

#include "engine/rhi/imgui_renderer.h"

#include "imgui_impl_dx12.h"

#include <d3d12.h>

namespace light::rhi
{
	class D12Device;

	class D12ImGuiRenderer: public ImGuiRenderer
	{
	public:
		// 通过 Imgui 继承
		bool Init(Device* device) override;

		void BeginFrame() override;

		void EndFrame(const RenderTarget& render_target) override;

		void Flush() override;

		void Shutdown() override;

	private:
		struct FrameContext
		{
			ID3D12CommandAllocator* command_allocator;
			UINT64 fence_value;
		};

		void WaitForLastSubmittedFrame()
		{
			FrameContext* frame_ctx = &frame_context_[frame_index_ % kNumFramesInFlight];

			UINT64 fence_value = frame_ctx->fence_value;
			if (fence_value == 0)
				return; // No fence was signaled

			frame_ctx->fence_value = 0;
			if (fence_->GetCompletedValue() >= fence_value)
				return;

			fence_->SetEventOnCompletion(fence_value, fence_event_);
			WaitForSingleObject(fence_event_, INFINITE);
		}

		FrameContext* WaitForNextFrameResources()
		{
			UINT next_frame_index = frame_index_ + 1;
			frame_index_ = next_frame_index;

			FrameContext* frame_ctx = &frame_context_[next_frame_index % kNumFramesInFlight];
			if (UINT64 fence_value = frame_ctx->fence_value; fence_value != 0) // means no fence was signaled
			{
				frame_ctx->fence_value = 0;
				fence_->SetEventOnCompletion(fence_value, fence_event_);

				WaitForSingleObject(fence_event_, INFINITE);
			}

			return frame_ctx;
		}

		// Data
		static constexpr int kNumFramesInFlight = 3;

		D12Device* d12_device_ = nullptr;
		FrameContext frame_context_[kNumFramesInFlight] = {};
		UINT frame_index_ = 0;
		Handle<ID3D12DescriptorHeap> pd3d_srv_desc_heap_ = nullptr;
		Handle<ID3D12GraphicsCommandList> pd3d_command_list_ = nullptr;
		Handle<ID3D12Fence> fence_ = nullptr;
		HANDLE fence_event_ = nullptr;
		UINT64 fence_last_signaled_value_ = 0;
	};
}

