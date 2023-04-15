#include "d12_command_list.h"

#include <array>
#include <memory>

#include "d12_device.h"
#include "d12_texture.h"
#include "d12_sampler.h"

#undef max
#undef min

namespace light::rhi
{
	D12CommandList::D12CommandList(D12Device* device, CommandListType type,CommandQueue* queue)
		: CommandList(type,queue)
		, device_(device)
		, upload_buffer_(device_)
		, current_pso_(nullptr)
	{
		ThrowIfFailed(device_->GetNative()->CreateCommandAllocator(ConvertCommandListType(type), IID_PPV_ARGS(&d3d12_command_allocator_)));
		ThrowIfFailed(device_->GetNative()->CreateCommandList(
			0, 
			ConvertCommandListType(type), 
			d3d12_command_allocator_.Get(), 
			nullptr, 
			IID_PPV_ARGS(&d3d12_command_list_)));
		
		for (size_t i = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; i <= D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER; ++i)
		{
			dynamic_descriptor_heaps_[i] = std::make_unique<DynamicDescriptorHeap>(device_, static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
			descriptr_heaps_[i] = nullptr;
		}

		for (size_t i = 0; i < 32; ++i)
		{
			buffer_gpu_virtual_address_[i] = ~0ul;
		}
	}

	D12CommandList::~D12CommandList()
	{

	}

	void D12CommandList::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* heap)
	{
		if(descriptr_heaps_[type] != heap)
		{
			descriptr_heaps_[type] = heap;
		}

		CommitDescriptorHeaps();
	}

	void D12CommandList::TransitionBarrier(Buffer* buffer, ResourceStates state_afeter, uint32_t subresource,
	                                       bool flush_barriers, bool permanent)
	{
		if(buffer->IsPermanentState())
		{
			return;
		}

		auto d12_buffer = CheckedCast<D12Buffer*>(buffer);
		resource_state_tracker_.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(
			d12_buffer->GetNative(),
			D3D12_RESOURCE_STATE_COMMON,
			ConvertResourceStates(state_afeter), subresource));

		if(flush_barriers)
		{
			FlushResourceBarriers();
		}
	}

	void D12CommandList::TransitionBarrier(Texture* texture, ResourceStates state_afeter, uint32_t subresource,
	                                       bool flush_barriers, bool permanent)
	{
		if (texture->IsPermanentState())
		{
			return;
		}

		auto d12_texture = CheckedCast<D12Texture*>(texture);
		resource_state_tracker_.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(
			d12_texture->GetNative(),
			D3D12_RESOURCE_STATE_COMMON,
			ConvertResourceStates(state_afeter), subresource));

		if(flush_barriers)
		{
			FlushResourceBarriers();
		}
	}

	void D12CommandList::ClearTexture(Texture* texture, const float* clear_value)
	{
		auto d12_texture = CheckedCast<D12Texture*>(texture);

		TransitionBarrier(d12_texture, ResourceStates::kRenderTarget);

		d3d12_command_list_->ClearRenderTargetView(d12_texture->GetRTV(), clear_value, 0, nullptr);

		TrackResource(d12_texture);

		ThrowIfFailed(device_->GetNative()->GetDeviceRemovedReason());
	}

	void D12CommandList::ClearTexture(Texture* texture, uint32_t mip_level, uint32_t array_slice,
		uint32_t num_array_slice, const float* clear_value)
	{
		auto d12_texture = CheckedCast<D12Texture*>(texture);

		TransitionBarrier(d12_texture, ResourceStates::kRenderTarget, CalcSubresource(mip_level, array_slice, texture->GetDesc().mip_levels));

		d3d12_command_list_->ClearRenderTargetView(d12_texture->GetRTV(Format::UNKNOWN,mip_level,array_slice,num_array_slice), clear_value, 0, nullptr);

		TrackResource(d12_texture);
	}

	void D12CommandList::ClearDepthStencilTexture(Texture* texture, ClearFlags clear_flags, float depth,
	                                              uint8_t stencil)
	{
		auto d12_texture = CheckedCast<D12Texture*>(texture);

		TransitionBarrier(d12_texture, ResourceStates::kDepthWrite);

		d3d12_command_list_->ClearDepthStencilView(d12_texture->GetDSV(), ConvertClearFlags(clear_flags), depth, stencil, 0, nullptr);

		TrackResource(d12_texture);
	}

	void D12CommandList::ClearDepthStencilTexture(Texture* texture, uint32_t mip_level, uint32_t array_slice,
		uint32_t num_array_slice, ClearFlags clear_flags, float depth, uint8_t stencil)
	{
		auto d12_texture = CheckedCast<D12Texture*>(texture);

		TransitionBarrier(d12_texture, ResourceStates::kDepthWrite, CalcSubresource(mip_level, array_slice, texture->GetDesc().mip_levels));

		d3d12_command_list_->ClearDepthStencilView(
			d12_texture->GetDSV(mip_level, array_slice, num_array_slice), 
			ConvertClearFlags(clear_flags), depth, stencil, 0, nullptr);

		TrackResource(d12_texture);
	}

	void D12CommandList::WriteTexture(Texture* texture, uint32_t first_subresource, uint32_t num_subresources, const std::vector<TextureSubresourceData>& data)
	{
		TrackResource(texture);
		TransitionBarrier(texture, ResourceStates::kCopyDest);

		auto d12_texture = CheckedCast<D12Texture*>(texture);

		UINT64 total_bytes = 0;

		UINT64 alloc_size = (sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64)) * num_subresources;
		auto mem = std::make_unique<uint8_t[]>(alloc_size);

		D3D12_RESOURCE_DESC resource_desc = d12_texture->GetNative()->GetDesc();
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layouts = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(mem.get());
		UINT64* row_size_in_bytes = reinterpret_cast<UINT64*>(layouts + num_subresources);
		UINT* num_rows = reinterpret_cast<UINT*>(row_size_in_bytes + num_subresources);

		device_->GetNative()->GetCopyableFootprints(&resource_desc, first_subresource, num_subresources, 0, layouts, num_rows, row_size_in_bytes, &total_bytes);

		UploadBuffer::Allocation allocation = upload_buffer_.Allocate(total_bytes, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
		for (uint32_t i = 0; i < num_subresources; ++i)
		{
			for (uint32_t depth = 0; depth < layouts[i].Footprint.Depth; ++depth)
			{
				for (uint32_t row = 0; row < num_rows[i]; ++row)
				{
					void* dest_address = (char*)allocation.cpu + layouts[i].Offset + layouts[i].Footprint.RowPitch * (row + depth * num_rows[i]);
					const void* src_address = data[i].data + data[i].row_pitch * (row + depth * num_rows[i]);
					memcpy(dest_address, src_address, std::min(layouts[i].Footprint.RowPitch, data[i].row_pitch));
				}
			}
		}

		for (uint32_t i = 0; i < num_subresources; ++i)
		{
			D3D12_TEXTURE_COPY_LOCATION dest_copy_location;
			dest_copy_location.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			dest_copy_location.SubresourceIndex = i + first_subresource;
			dest_copy_location.pResource = d12_texture->GetNative();

			D3D12_TEXTURE_COPY_LOCATION src_copy_location;
			src_copy_location.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			src_copy_location.PlacedFootprint = layouts[i];
			src_copy_location.pResource = allocation.upload_resource;

			d3d12_command_list_->CopyTextureRegion(&dest_copy_location, 0, 0, 0, &src_copy_location, nullptr);
		}
	}

	void D12CommandList::WriteBuffer(Buffer* buffer, const uint8_t* data, uint64_t size, uint64_t dest_offset_bytes)
	{
		if (size == 0)
		{
			return;
		}
		
		auto d12_buffer = CheckedCast<D12Buffer*>(buffer);

		if (buffer->GetDesc().cpu_access == CpuAccess::kWrite)
		{
			memcpy(d12_buffer->GetCpuAddress(), data, size);
		}
		else
		{
			UploadBuffer::Allocation allocation = upload_buffer_.Allocate(size, 1);

			memcpy(allocation.cpu, data, size);

			TransitionBarrier(buffer, ResourceStates::kCopyDest,~0,true);

			d3d12_command_list_->CopyBufferRegion(
				d12_buffer->GetNative(),
				dest_offset_bytes,
				allocation.upload_resource,
				allocation.offset, size);
		}
	}

	void D12CommandList::SetGraphicsDynamicConstantBuffer(uint32_t parameter_index, size_t bytes, const void* data)
	{
		UploadBuffer::Allocation allocation = upload_buffer_.Allocate(bytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

		memcpy(allocation.cpu, data, bytes);

		//if (buffer_states_[parameter_index] != allocation.gpu)
		{
			d3d12_command_list_->SetGraphicsRootConstantBufferView(parameter_index, allocation.gpu);
		}

		buffer_states_[parameter_index] = allocation.gpu;
	}

	void D12CommandList::SetGraphics32BitConstants(uint32_t parameter_index, uint32_t num_constants,
		const void* constants)
	{
		d3d12_command_list_->SetGraphicsRoot32BitConstants(parameter_index, num_constants, constants, 0);
	}

	void D12CommandList::SetBufferView(uint32_t parameter_index, Buffer* buffer, uint32_t offset,
		ResourceStates state_after)
	{
		TrackResource(buffer);

		TransitionBarrier(buffer, state_after);

		auto d12_buffer = CheckedCast<D12Buffer*>(buffer);
		buffer_gpu_virtual_address_[parameter_index] = d12_buffer->GetNative()->GetGPUVirtualAddress() + offset;
	}

	void D12CommandList::SetConstantBufferView(uint32_t parameter_index, uint32_t descriptor_offset, Buffer* buffer,ResourceStates state_after)
	{
		TrackResource(buffer);

		TransitionBarrier(buffer, state_after);

		auto d12_buffer = CheckedCast<D12Buffer*>(buffer);
		dynamic_descriptor_heaps_[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(
			parameter_index,descriptor_offset,1,d12_buffer->GetCBV());
	}

	void D12CommandList::SetStructuredBufferView(uint32_t parameter_index, uint32_t descriptor_offset, Buffer* buffer,
		uint32_t offset, ResourceStates state_after)
	{
		SetStructuredBufferView(parameter_index, descriptor_offset, buffer, offset, buffer->GetDesc().size_in_bytes, state_after);
	}

	void D12CommandList::SetStructuredBufferView(uint32_t parameter_index, uint32_t descriptor_offset, Buffer* buffer,
		uint32_t offset, uint32_t byte_size, ResourceStates state_after)
	{
		TrackResource(buffer);

		TransitionBarrier(buffer, state_after);

		auto d12_buffer = CheckedCast<D12Buffer*>(buffer);
		dynamic_descriptor_heaps_[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(
			parameter_index, descriptor_offset, 1, d12_buffer->GetSBV(offset, byte_size));
	}

	void D12CommandList::SetUnorderedAccessBufferView(uint32_t parameter_index, uint32_t descriptor_offset,
		Buffer* buffer, uint32_t offset, ResourceStates state_after)
	{
		
		SetUnorderedAccessBufferView(parameter_index, descriptor_offset, buffer, offset, buffer->GetDesc().size_in_bytes, state_after);
	}

	void D12CommandList::SetUnorderedAccessBufferView(uint32_t parameter_index, uint32_t descriptor_offset,
		Buffer* buffer, uint32_t offset, uint32_t byte_size, ResourceStates state_after)
	{
		TrackResource(buffer);

		TransitionBarrier(buffer, state_after);

		auto d12_buffer = CheckedCast<D12Buffer*>(buffer);
		dynamic_descriptor_heaps_[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(
			parameter_index, descriptor_offset, 1, d12_buffer->GetUBV(offset, byte_size));
	}

	void D12CommandList::SetShaderResourceView(uint32_t parameter_index, uint32_t descriptor_offset, Texture* texture,
		Format format, TextureDimension dimension, uint32_t mip_level, uint32_t num_mip_levels, uint32_t array_slice,
		uint32_t num_array_slices, ResourceStates state_after)
	{
		TrackResource(texture);
		
		TransitionBarrier(texture, state_after);

		auto d12_texture = CheckedCast<D12Texture*>(texture);
		dynamic_descriptor_heaps_[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(
			parameter_index, descriptor_offset, 1,
			d12_texture->GetSRV(format, dimension, mip_level, num_mip_levels, array_slice, num_array_slices));
	}

	void D12CommandList::SetSampler(uint32_t parameter_index, uint32_t descriptor_offset, Sampler* sampler)
	{
		TrackResource(sampler);

		auto d12_sampler = CheckedCast<D12Sampler*>(sampler);
		dynamic_descriptor_heaps_[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER]->StageDescriptors(parameter_index, descriptor_offset, 1, d12_sampler->GetDescriptorHandle());
	}

	void D12CommandList::SetGraphicsPipeline(GraphicsPipeline* pso)
	{
		if(current_pso_ != pso)
		{
			auto d12_pso = CheckedCast<D12GraphicsPipeline*>(pso);

			auto root_signature = d12_pso->GetRootSignature();
			d3d12_command_list_->SetGraphicsRootSignature(root_signature->GetNative());
			
			for (auto& dynamic_descriptor_heap : dynamic_descriptor_heaps_)
			{
				dynamic_descriptor_heap->ParseRootSignature(root_signature);
			}

			d3d12_command_list_->SetPipelineState(d12_pso->GetNative());
		}

		TrackResource(pso);
	}

	void D12CommandList::SetPrimitiveTopology(PrimitiveTopology primitive_topology)
	{
		d3d12_command_list_->IASetPrimitiveTopology(ConvertPrimitiveTopology(primitive_topology));
	}

	void D12CommandList::SetVertexBuffers(const std::vector<BufferHandle>& buffers)
	{
		std::vector<D3D12_VERTEX_BUFFER_VIEW> views(buffers.size());
		

		for(int i = 0;i<buffers.size();++i)
		{
			auto& buffer = buffers[i];
			auto& view = views[i];

			const BufferDesc& desc = buffer->GetDesc();

			CHECK(desc.type == BufferType::kVertex, "buffer的Type不是VertexBuffer");

			auto d12_buffer = CheckedCast<D12Buffer*>(buffer.Get());

			TrackResource(buffer);
			TransitionBarrier(buffer, ResourceStates::kVertexAndConstantBuffer);

			view.BufferLocation = d12_buffer->GetNative()->GetGPUVirtualAddress();
			view.SizeInBytes = desc.size_in_bytes;
			view.StrideInBytes = desc.stride;
		}

		d3d12_command_list_->IASetVertexBuffers(0, static_cast<UINT>(views.size()), views.data());
	}

	void D12CommandList::SetVertexBuffer(uint32_t slot, Buffer* buffer)
	{
		const BufferDesc& desc = buffer->GetDesc();

		CHECK(desc.type == BufferType::kVertex, "buffer的Type不是VertexBuffer");

		auto d12_buffer = CheckedCast<D12Buffer*>(buffer);

		TrackResource(buffer);
		TransitionBarrier(buffer, ResourceStates::kVertexAndConstantBuffer);

		D3D12_VERTEX_BUFFER_VIEW view{};
		view.BufferLocation = d12_buffer->GetNative()->GetGPUVirtualAddress();
		view.SizeInBytes = desc.size_in_bytes;
		view.StrideInBytes = desc.stride;

		d3d12_command_list_->IASetVertexBuffers(slot, 1, &view);
	}

	void D12CommandList::SetDynamicVertexBuffer(uint32_t slot, void* data, uint32_t size, uint32_t stride)
	{
		UploadBuffer::Allocation allocation = upload_buffer_.Allocate(size,1);
		
		memcpy(allocation.cpu, data, size);
		
		D3D12_VERTEX_BUFFER_VIEW view{};
		view.BufferLocation = allocation.gpu;
		view.SizeInBytes = size;
		view.StrideInBytes = stride;
		
		d3d12_command_list_->IASetVertexBuffers(slot, 1, &view);
	}

	void D12CommandList::SetIndexBuffer(Buffer* buffer)
	{
		const BufferDesc& desc = buffer->GetDesc();

		CHECK(desc.type == BufferType::kIndex,"buffer的Type不是IndexBuffer");

		auto d12_buffer = CheckedCast<D12Buffer*>(buffer);

		TrackResource(buffer);
		TransitionBarrier(buffer, ResourceStates::kIndexBuffer);

		D3D12_INDEX_BUFFER_VIEW view{};
		view.BufferLocation = d12_buffer->GetNative()->GetGPUVirtualAddress();
		view.SizeInBytes = static_cast<UINT>(desc.size_in_bytes);
		view.Format = GetDxgiFormatMapping(buffer->GetDesc().format).srv_format;
		d3d12_command_list_->IASetIndexBuffer(&view);
	}

	void D12CommandList::SetRenderTarget(const RenderTarget& render_target)
	{
		std::array<D3D12_CPU_DESCRIPTOR_HANDLE,static_cast<uint32_t>(AttachmentPoint::kNumAttachmentPoints)> render_target_descriptors{};
		uint32_t num_render_target = 0;
		const auto& textures = render_target.GetAttachments();

		//查找所有color target
		for (uint32_t i = 0; i < static_cast<uint32_t>(AttachmentPoint::kDepthStencil); ++i)
		{
			const auto& attachment = textures[i];
			if (attachment.texture)
			{
				auto d12_texture = CheckedCast<D12Texture*>(attachment.texture.Get());

				TransitionBarrier(d12_texture, ResourceStates::kRenderTarget);

				if(attachment.IsAllSubresource())
				{
					render_target_descriptors[num_render_target++] = d12_texture->GetRTV();
				}
				else
				{
					render_target_descriptors[num_render_target++] = 
						d12_texture->GetRTV(attachment.format, attachment.mip_level, attachment.array_slice, attachment.num_array_slice);
				}

				TrackResource(d12_texture);
			}
		}

		D3D12_CPU_DESCRIPTOR_HANDLE depth_stencil_descriptor{0};

		const auto& attachment = render_target.GetAttachment(AttachmentPoint::kDepthStencil);
		if(attachment.texture)
		{
			auto d12_depth_texture = CheckedCast<D12Texture*>(attachment.texture.Get());

			TransitionBarrier(d12_depth_texture, ResourceStates::kDepthWrite);

			if(attachment.IsAllSubresource())
			{
				depth_stencil_descriptor = d12_depth_texture->GetDSV();
			}
			else
			{
				depth_stencil_descriptor = d12_depth_texture->GetDSV(attachment.mip_level, attachment.array_slice, attachment.num_array_slice);
			}

			TrackResource(d12_depth_texture);
		}

		FlushResourceBarriers();

		D3D12_CPU_DESCRIPTOR_HANDLE* dsv = depth_stencil_descriptor.ptr != 0 ? &depth_stencil_descriptor : nullptr;
		d3d12_command_list_->OMSetRenderTargets(num_render_target, render_target_descriptors.data(), false, dsv);
	}

	void D12CommandList::SetViewport(const Viewport& viewport)
	{
		SetViewports({ viewport });
	}

	void D12CommandList::SetViewports(const std::vector<Viewport>& viewports)
	{
		std::vector<D3D12_VIEWPORT> d12_viewports;
		d12_viewports.reserve(viewports.size());

		for (auto& viewport : viewports)
		{
			d12_viewports.emplace_back(ConvertViewport(viewport));
		}

		d3d12_command_list_->RSSetViewports(d12_viewports.size(), d12_viewports.data());
	}

	void D12CommandList::SetScissorRect(const Rect& rect)
	{
		SetScissorRects({ rect });
	}

	void D12CommandList::SetScissorRects(const std::vector<Rect>& rects)
	{
		std::vector<D3D12_RECT> d12_rects;
		d12_rects.reserve(rects.size());

		for (auto& rect : rects)
		{
			d12_rects.emplace_back(ConvertRect(rect));
		}

		d3d12_command_list_->RSSetScissorRects(d12_rects.size(), d12_rects.data());
	}

	void D12CommandList::ExecuteCommandList()
	{
		queue_->ExecuteCommandList(this);
	}

	bool D12CommandList::Close(CommandList* pending_command_list)
	{
		//刷新剩余资源屏障
		FlushResourceBarriers();

		//刷新挂起的资源屏障
		uint32_t num_pending_barries = resource_state_tracker_.FlushPendingResourceBarriers(CheckedCast<D12CommandList*>(pending_command_list));

		// 提交最终资源导全局状态
		resource_state_tracker_.CommitFinalResourceStates();

		d3d12_command_list_->Close();

		return num_pending_barries > 0;
	}

	void D12CommandList::Close()
	{
		FlushResourceBarriers();
		d3d12_command_list_->Close();
	}

	void D12CommandList::Reset()
	{
		ThrowIfFailed(d3d12_command_allocator_->Reset());
		ThrowIfFailed(d3d12_command_list_->Reset(d3d12_command_allocator_.Get(),nullptr));

		track_resources_.clear();

		upload_buffer_.Rest();

		current_pso_ = nullptr;

		for (auto& dynamic_descriptor_heap : dynamic_descriptor_heaps_)
		{
			dynamic_descriptor_heap->Rest();
		}
	}

	void D12CommandList::DrawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t start_index,
	                                 int32_t base_vertex, uint32_t start_instance)
	{
		FlushResourceBarriers();

		for (auto& dynamic_descriptor_heap : dynamic_descriptor_heaps_)
		{
			dynamic_descriptor_heap->CommitStatedDescriptorsForDraw(this);
		}

		d3d12_command_list_->DrawIndexedInstanced(index_count, instance_count, start_index, base_vertex, start_instance);
	}

	void D12CommandList::CommitDescriptorHeaps()
	{
		uint32_t num_heaps = 0;
		ID3D12DescriptorHeap* heap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		for (size_t type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; type <= D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER; ++type)
		{
			if(descriptr_heaps_[type])
			{
				heap[num_heaps++] = descriptr_heaps_[type];
			}
		}

		d3d12_command_list_->SetDescriptorHeaps(num_heaps, heap);
	}

	void D12CommandList::TrackResource(Resource* resource)
	{
		track_resources_.emplace_back(resource);
	}

	void D12CommandList::FlushResourceBarriers()
	{
		resource_state_tracker_.FlushResourceBarriers(this);
	}
}
