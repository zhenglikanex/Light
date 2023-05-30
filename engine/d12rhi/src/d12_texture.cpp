#include "d12_texture.h"
#include "d12_convert.h"
#include "d12_device.h"
#include "resource_state_tracker.h"

namespace light::rhi
{
	D12Texture::D12Texture(D12Device* device, const TextureDesc& desc,const ClearValue* clear_value)
		: Texture(desc)
		, device_(device)
		, resource_(nullptr)
	{
		auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto res_desc = CD3DX12_RESOURCE_DESC::Tex2D(GetDxgiFormatMapping(desc.format).rtv_format, desc.width, desc.height, desc.array_size, desc.mip_levels);

		if(desc.is_render_target)
		{
			res_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		auto& format_info = GetFormatInfo(desc.format);
		if (format_info.has_depth)
		{
			res_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}

		if(clear_value)
		{
			D3D12_CLEAR_VALUE d12_clear_value;
			d12_clear_value.Format = GetDxgiFormatMapping(desc.format).rtv_format;

			if(desc.is_render_target)
			{
				memcpy(d12_clear_value.Color, clear_value->color, sizeof(clear_value->color));
			}
			else
			{
				d12_clear_value.DepthStencil.Depth = clear_value->depth_stencil.depth;
				d12_clear_value.DepthStencil.Stencil = clear_value->depth_stencil.stencil;
			}

			device_->GetNative()->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &res_desc, D3D12_RESOURCE_STATE_COMMON, &d12_clear_value, IID_PPV_ARGS(&resource_));

			clear_value_ = std::make_unique<ClearValue>(*clear_value);
		}
		else
		{
			device_->GetNative()->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &res_desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resource_));
		}
	}

	D12Texture::D12Texture(D12Device* device, const TextureDesc& desc, ID3D12Resource* native)
		: Texture(desc)
		, device_(device)
		, resource_(native)
	{

	}

	D12Texture::~D12Texture()
	{
		ResourceStateTracker::RemoveGlobalResourceState(resource_);
	}

	const ClearValue* D12Texture::GetClearValue() const
	{
		if (clear_value_)
		{
			return clear_value_.get();
		}
		else
		{
			return nullptr;
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D12Texture::GetRTV()
	{
		size_t hash = 0;

		auto it = rtv_map_.find(hash);
		if(it != rtv_map_.end())
		{
			return it->second.GetDescriptorHandle();
		}

		DescriptorAllocation allocation = device_->AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
		D3D12_CPU_DESCRIPTOR_HANDLE handle = allocation.GetDescriptorHandle();
		device_->GetNative()->CreateRenderTargetView(resource_, nullptr, handle);
		rtv_map_.emplace(hash, std::move(allocation));

		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D12Texture::GetRTV(Format format, uint32_t mip_level, uint32_t array_slice, uint32_t num_array_slices)
	{
		format = format == Format::UNKNOWN ? desc_.format : format;

		size_t hash = 0;

		HashCombine(hash, static_cast<uint8_t>(format));
		HashCombine(hash, mip_level);
		HashCombine(hash, array_slice);
		HashCombine(hash, num_array_slices);

		auto it = rtv_map_.find(hash);
		if (it != rtv_map_.end())
		{
			return it->second.GetDescriptorHandle();
		}

		D3D12_RENDER_TARGET_VIEW_DESC rtv_desc{};
		rtv_desc.Format = GetDxgiFormatMapping(format).rtv_format;

		switch (desc_.dimension) {
		case TextureDimension::kTexture1D:
			rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
			rtv_desc.Texture1D.MipSlice = mip_level;
			break;
			
		case TextureDimension::kTexture1DArray:
			rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
			rtv_desc.Texture1DArray.MipSlice = mip_level;
			rtv_desc.Texture1DArray.FirstArraySlice = array_slice;
			rtv_desc.Texture1DArray.ArraySize = num_array_slices;
			break;

		case TextureDimension::kTexture2D: 
			rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtv_desc.Texture2D.MipSlice = mip_level;
			break;

		case TextureDimension::kTexture2DArray:
		case TextureDimension::kTextureCube:
		case TextureDimension::kTextureCubeArray:
			rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
			rtv_desc.Texture2DArray.MipSlice = mip_level;
			rtv_desc.Texture2DArray.FirstArraySlice = array_slice;
			rtv_desc.Texture2DArray.ArraySize = num_array_slices;
			break;
		
		case TextureDimension::kTexture2DMS:
			rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
			break;
			
		case TextureDimension::kTexture2DMSArray: 
			rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
			rtv_desc.Texture2DMSArray.FirstArraySlice = array_slice;
			rtv_desc.Texture2DMSArray.ArraySize = num_array_slices;
			break;
		case TextureDimension::kTexture3D: 
			rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
			rtv_desc.Texture3D.FirstWSlice = array_slice;
			rtv_desc.Texture3D.WSize = num_array_slices;
			rtv_desc.Texture3D.MipSlice = mip_level;
			break;
		case TextureDimension::kUnknown:
		default:
			assert(false);
		}

		DescriptorAllocation allocation = device_->AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
		D3D12_CPU_DESCRIPTOR_HANDLE handle = allocation.GetDescriptorHandle();
		device_->GetNative()->CreateRenderTargetView(resource_, &rtv_desc, handle);
		rtv_map_.emplace(hash, std::move(allocation));

		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D12Texture::GetDSV()
	{
		size_t hash = 0;

		auto it = dsv_map_.find(hash);
		if(it != dsv_map_.end())
		{
			return it->second.GetDescriptorHandle();
		}

		DescriptorAllocation allocation = device_->AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
		D3D12_CPU_DESCRIPTOR_HANDLE handle = allocation.GetDescriptorHandle();
		device_->GetNative()->CreateDepthStencilView(resource_,nullptr, handle);
		dsv_map_.emplace(hash, std::move(allocation));

		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D12Texture::GetDSV(uint32_t mip_level, uint32_t array_slice, uint32_t num_array_slices)
	{
		size_t hash = 0;

		HashCombine(hash, mip_level);
		HashCombine(hash, array_slice);
		HashCombine(hash, num_array_slices);

		auto it = dsv_map_.find(hash);
		if(it != dsv_map_.end())
		{
			return it->second.GetDescriptorHandle();
		}

		D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
		dsv_desc.Format = GetDxgiFormatMapping(desc_.format).rtv_format;

		switch (desc_.dimension)
		{
		case TextureDimension::kTexture1D:
			dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
			dsv_desc.Texture1D.MipSlice = mip_level;
			break;
		case TextureDimension::kTexture1DArray:
			dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
			dsv_desc.Texture1DArray.MipSlice = mip_level;
			dsv_desc.Texture1DArray.FirstArraySlice = array_slice;
			dsv_desc.Texture1DArray.ArraySize = num_array_slices;
			break;
		case TextureDimension::kTexture2D:
			dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsv_desc.Texture1D.MipSlice = mip_level;
			break;
		case TextureDimension::kTexture2DArray:
		case TextureDimension::kTextureCube:
		case TextureDimension::kTextureCubeArray:
			dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
			dsv_desc.Texture2DArray.MipSlice = mip_level;
			dsv_desc.Texture2DArray.FirstArraySlice = array_slice;
			dsv_desc.Texture2DArray.ArraySize = num_array_slices;
			break;

		case TextureDimension::kTexture2DMS:
			dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
			break;

		case TextureDimension::kTexture2DMSArray:
			dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
			dsv_desc.Texture2DMSArray.FirstArraySlice = array_slice;
			dsv_desc.Texture2DMSArray.ArraySize = num_array_slices;
			break;
		case TextureDimension::kTexture3D:
		case TextureDimension::kUnknown:
		default:
			assert(false);
		}

		DescriptorAllocation allocation = device_->AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
		D3D12_CPU_DESCRIPTOR_HANDLE handle = allocation.GetDescriptorHandle();
		device_->GetNative()->CreateDepthStencilView(resource_, &dsv_desc, handle);
		dsv_map_.emplace(hash, std::move(allocation));

		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D12Texture::GetSRV(Format format, TextureDimension dimension, uint32_t mip_level, uint32_t num_mip_levels, uint32_t array_slice,
		uint32_t num_array_slices)
	{
		format = format == Format::UNKNOWN ? desc_.format : format;

		size_t hash = 0;

		HashCombine(hash, static_cast<uint8_t>(format));
		HashCombine(hash, mip_level);
		HashCombine(hash, array_slice);
		HashCombine(hash, num_array_slices);

		auto it = srv_map_.find(hash);
		if (it != srv_map_.end())
		{
			return it->second.GetDescriptorHandle();
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
		srv_desc.Format = GetDxgiFormatMapping(format).srv_format;
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		switch (dimension) {
		case TextureDimension::kTexture1D:
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
			srv_desc.Texture1D.MostDetailedMip = mip_level;
			srv_desc.Texture1D.MipLevels = num_mip_levels;
			srv_desc.Texture1D.ResourceMinLODClamp = mip_level;
			break;

		case TextureDimension::kTexture1DArray:
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
			srv_desc.Texture1DArray.MostDetailedMip = mip_level;
			srv_desc.Texture1DArray.MipLevels = num_mip_levels;
			srv_desc.Texture1DArray.FirstArraySlice = array_slice;
			srv_desc.Texture1DArray.ArraySize = num_array_slices;
			srv_desc.Texture1DArray.ResourceMinLODClamp = mip_level;
			break;

		case TextureDimension::kTexture2D:
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srv_desc.Texture2D.MostDetailedMip = mip_level;
			srv_desc.Texture2D.MipLevels = num_mip_levels;
			srv_desc.Texture2D.ResourceMinLODClamp = mip_level;
			srv_desc.Texture2D.PlaneSlice = 0;
			break;

		case TextureDimension::kTexture2DArray:
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			srv_desc.Texture2DArray.MostDetailedMip = mip_level;
			srv_desc.Texture2DArray.MipLevels = num_mip_levels;
			srv_desc.Texture2DArray.FirstArraySlice = array_slice;
			srv_desc.Texture2DArray.ArraySize = num_array_slices;
			break;

		case TextureDimension::kTextureCube:
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srv_desc.TextureCube.MostDetailedMip = mip_level;
			srv_desc.TextureCube.MipLevels = num_mip_levels;
			srv_desc.TextureCube.ResourceMinLODClamp = mip_level;
			break;
		case TextureDimension::kTextureCubeArray:
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
			srv_desc.TextureCubeArray.MostDetailedMip = mip_level;
			srv_desc.TextureCubeArray.MipLevels = num_mip_levels;
			srv_desc.TextureCubeArray.First2DArrayFace = array_slice;
			srv_desc.TextureCubeArray.NumCubes = num_array_slices / 6;
			break;
		case TextureDimension::kTexture2DMS:
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
			break;

		case TextureDimension::kTexture2DMSArray:
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
			srv_desc.Texture2DMSArray.FirstArraySlice = array_slice;
			srv_desc.Texture2DMSArray.ArraySize = num_array_slices;
			break;
		case TextureDimension::kTexture3D:
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			srv_desc.Texture3D.MostDetailedMip = mip_level;
			srv_desc.Texture3D.MipLevels = num_mip_levels;
			break;
		case TextureDimension::kUnknown:
		default:
			assert(false);
		}

		DescriptorAllocation allocation = device_->AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
		D3D12_CPU_DESCRIPTOR_HANDLE handle = allocation.GetDescriptorHandle();
		device_->GetNative()->CreateShaderResourceView(resource_, &srv_desc, handle);
		srv_map_.emplace(hash, std::move(allocation));

		return handle;
	}

	void* D12Texture::GetTextureID()
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = GetSRV(Format::UNKNOWN, TextureDimension::kTexture2D, 0, 1, 0, 1);
		return reinterpret_cast<void*>(handle.ptr);
	}
}
