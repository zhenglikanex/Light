#pragma once

#include "types.h"
#include "engine/core/base.h"
#include "render_target.h"

namespace light::rhi
{
	class Device;

	class SwapChain : public RefCounter
	{
	public:
		static constexpr uint32_t kBufferCount = 2;
		static constexpr Format kBufferForamt = Format::RGBA8_UNORM;

		virtual uint32_t Present() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual RenderTarget GetRenderTarget() = 0;

		virtual uint32_t GetWidth() = 0;

		virtual uint32_t GetHeight() = 0;

		virtual void SetVSync(bool vsync) = 0;
	};

	using SwapChainHandle = Ref<SwapChain>;
}