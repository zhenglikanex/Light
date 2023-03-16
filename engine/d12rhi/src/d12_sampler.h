#pragma once

#include "engine/rhi/sampler.h"

#include "descriptor_allocator.h"

namespace light::rhi
{
	class D12Sampler : public Sampler
	{
	public:
		D12Sampler(D12Device* device, const SamplerDesc& desc);

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const;
	private:
		DescriptorAllocation allocation;
	};
}