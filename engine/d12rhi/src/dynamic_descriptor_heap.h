#pragma once

#include <cstdint>
#include <queue>
#include <functional>

#include "engine/rhi/resource.h"

#include "d3dx12.h"


namespace light::rhi
{
	class D12Device;
	class D12CommandList;
	class RootSignature;

	class DynamicDescriptorHeap
	{
	public:
		DynamicDescriptorHeap(D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heap_type, uint32_t heap_size = 1024);

		~DynamicDescriptorHeap();

		void StageDescriptors(uint32_t parameter_index, uint32_t offset, uint32_t num_descriptors,
			D3D12_CPU_DESCRIPTOR_HANDLE src_descriptors);

		D3D12_GPU_DESCRIPTOR_HANDLE CopyDescriptor(D12CommandList* command_list, D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor);

		void CommitStatedDescriptorsForDraw(D12CommandList* command_list);

		void CommitStatedDescriptorsForCompute(D12CommandList* command_list);

		void ParseRootSignature(const RootSignature* root_signature);

		void Rest();
	private:
		ID3D12DescriptorHeap* RequestDescriptorHeap();

		Handle<ID3D12DescriptorHeap> CreateDescriptorHeap();

		// ������Ҫ�ύ��GPU�ɼ��ѵ�����
		uint32_t ComputeStaleDescriptorCount() const;

		void CommitDescriptorTables(D12CommandList* command_list,
			std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> set_func);

		//ÿ����ǩ�������������������
		//32λ�������ڸ�����Ϊ���������ĸ�����������
		static constexpr uint32_t kMaxDescriptorTables = 32;

		
		struct DescriptorTableCache
		{
			void Reset()
			{
				num_descriptors = 0;
				base_descriptor = nullptr;
			}

			uint32_t num_descriptors = 0;
			D3D12_CPU_DESCRIPTOR_HANDLE* base_descriptor = nullptr;
		};

		D12Device* device_;

		//��Ч�����ͣ�
		//	D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		//	D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
		//���������͵���������Ҫ�󶨵�gpu�ɼ���heap
		D3D12_DESCRIPTOR_HEAP_TYPE heap_type_;

		uint32_t heap_size_;

		uint32_t descriptor_handle_increment_size_;

		//�ݴ����е�cpu�ɼ���������
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> cpu_descriptor_handles_;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> src_descritpor_range_starts_;

		DescriptorTableCache descriptor_table_cache_[kMaxDescriptorTables];

		// root signature��Ӧ����������������
		uint32_t descriptor_table_bit_mask_;

		// ��Ҫ�ĸ��µ�root signature�����������ĸ���������������
		uint32_t stale_descriptor_table_bit_mask_;

		std::queue<Handle<ID3D12DescriptorHeap>> descriptor_heap_pool_;
		std::queue<Handle<ID3D12DescriptorHeap>> available_descriptor_heaps_;

		ID3D12DescriptorHeap* current_descriptor_heap_;
		CD3DX12_GPU_DESCRIPTOR_HANDLE current_gpu_descriptor_handle_;
		CD3DX12_CPU_DESCRIPTOR_HANDLE current_cpu_descriptor_handle_;

		uint32_t num_free_handles_;
	};
}