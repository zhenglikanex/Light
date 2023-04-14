#pragma once

#include <dxgi1_5.h>
#include <dxgi1_2.h>
#include <wrl.h>
#include <Windows.h>

#include "engine/rhi/swap_chain.h"

#include "d3dx12.h"
#include "d12_texture.h"

namespace light::rhi
{
	class CommandQueue;
	class D12Device;
	class CommandList;

	class D12SwapChain final : public SwapChain
	{
	public:

		D12SwapChain(D12Device* device, HWND hwnd);

		uint32_t Present() override;

		void Resize(uint32_t width, uint32_t height) override;

		RenderTarget GetRenderTarget() override;

		uint32_t GetWidth() override { return width_; }

		uint32_t GetHeight() override { return height_; }

		void SetVSync(bool vsync) override { vsync_ = vsync; }
	private:
		void UpdateRenderTargetViews();

		D12Device* device_;
		CommandQueue* command_queue_;
		HWND hwnd_;
		uint32_t width_;
		uint32_t height_;
		Handle<IDXGISwapChain4> dxgi_swap_chain_;
		TextureHandle back_buffer_textures_[kBufferCount];
		uint32_t current_back_buffer_index_;
		uint64_t fence_values_[kBufferCount];
		bool vsync_;
		bool allow_tearing_;
	};
}