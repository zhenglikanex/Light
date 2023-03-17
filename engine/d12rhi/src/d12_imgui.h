#pragma once

#include "engine/rhi/imgui.h"

namespace light::rhi
{
	class D12Device;

	class D12Imgui : public Imgui
	{
	public:
		// 通过 Imgui 继承
		virtual void Init(Device* device) override;

		virtual void OnUpdate() override;

		virtual void Shutdown() override;
	private:
		D12Device* d12_device_;
	};
}

extern "C" inline __declspec(dllexport)  light::rhi::D12Imgui* CreateImgui()
{
	return new light::rhi::D12Imgui();
}