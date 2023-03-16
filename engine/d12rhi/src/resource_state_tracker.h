#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>

#include "engine/rhi/resource.h"

#include "d3d12.h"

namespace light::rhi
{
	class D12CommandList;

	//���ڿ�Խ��������б��Ͷ��̸߳�����Դ��״̬
	//ȷ����ȷ����Դ״̬ת������ʹ��Դ�ڲ�ͬ�߳�
	//���Բ�ͬ��״̬ʹ�á�
	//����ָ�����������б������Խ����̹߳���
	//(�����б��ǵ��̲߳�����)
	//����ResourceStateTracker�ύ��Դת��ʱ��
	//���ȼ�����Դ֮ǰ�Ƿ��Ѿ��ڵ�ǰ�������ʹ�ù���
	//�����Դ��δ�������б���ʹ�ã������Ὣת������
	//���ӵ�������Դ����(δֱ��ֱ�����ӵ������б�)
	//������Դ�ĺ���״̬����֪���С��´�������ͬ��Դʱ
	//��ʹ����Դ����֪״̬��Ϊת��֮ǰ��״̬��
	//�����������ӵ������б���
	//�������б��ύ���������ʱ�����������������Դ��
	//ȫ��״̬���бȽϡ����ȫ��״̬�͹���״̬��ͬ��
	//�򽫹�����������ӵ���һ�������б�(ר�������ύ�м��״̬ת��)
	//���б����뵽��������У�λ������ִ�е������б�֮ǰ��

	class ResourceStateTracker
	{
	public:
		static std::mutex s_global_mutex;

		void ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier);

		void FlushResourceBarriers(D12CommandList* command_list);

		uint32_t FlushPendingResourceBarriers(D12CommandList* command_list);

		void CommitFinalResourceStates();

		void Reset();

		void AddGlobalResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state);
	private:
		struct ResourceState
		{
			void SetSubresourceState(UINT subresource,D3D12_RESOURCE_STATES state)
			{
				if(subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
				{
					this->state = state;
					subresource_state.clear();
				}
				else
				{
					subresource_state[subresource] = state;
				}
			}

			D3D12_RESOURCE_STATES GetSubresourceState(UINT subresource) const
			{
				auto it = subresource_state.find(subresource);
				if(it != subresource_state.end())
				{
					return it->second;
				}

				return state;
			}

			D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
			std::unordered_map<UINT, D3D12_RESOURCE_STATES> subresource_state;
		};

		using ResourceStateMap = std::unordered_map<ID3D12Resource*, ResourceState>;

		//��δ�������е�ʹ�ù�����Դ,���������б�ִ��ǰ�����������ִ��
		//(ͨ�����ӵ�����ת���õ������б�)
		std::vector<D3D12_RESOURCE_BARRIER> pending_resource_barriers_;

		// ��Դ���ϣ���Ҫ���ӵ������б�ִ��
		std::vector<D3D12_RESOURCE_BARRIER> resource_barriers_;

		ResourceStateMap final_resource_state_;

		static ResourceStateMap s_global_resource_state_;

	};
}
