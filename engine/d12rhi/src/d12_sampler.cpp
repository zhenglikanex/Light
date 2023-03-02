#include "d12_sampler.h"

#include "d12_device.h"
#include "d12_convert.h"

namespace light::rhi
{
	D12Sampler::D12Sampler(D12Device* device, const SamplerDesc& desc)
		: Sampler(desc)
	{
		allocation = device->AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1);

		D3D12_SAMPLER_DESC d12_desc;
		d12_desc.Filter = ConvertFilter(desc.filter);
		d12_desc.AddressU = ConvertTextureAddressMode(desc.u);
		d12_desc.AddressW = ConvertTextureAddressMode(desc.w);
		d12_desc.AddressV = ConvertTextureAddressMode(desc.v);
		d12_desc.MipLODBias = desc.mip_lod_bias;
		d12_desc.MaxAnisotropy = desc.max_anisotropy;
		memcpy(d12_desc.BorderColor, desc.border_color, sizeof(d12_desc.BorderColor));
		d12_desc.MinLOD = desc.min_lod;
		d12_desc.MaxLOD = desc.max_lod;
		
		device->GetNative()->CreateSampler(&d12_desc, allocation.GetDescriptorHandle());
	}
	D3D12_CPU_DESCRIPTOR_HANDLE D12Sampler::GetDescriptorHandle() const
	{
		return allocation.GetDescriptorHandle();
	}
}