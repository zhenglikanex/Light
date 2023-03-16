#pragma once

#include "engine/event/event.h"

namespace light
{
	// �ò�����ﲻͬģ��֮�����Ⱦ˳��͸��ǹ�ϵ
	// Ӱ��ģ��֮�����Ⱦ˳����¼�����
	// ����UI���3D��ĸ��ǹ�ϵ����Ⱦ˳��
	class Layer
	{
	public:
		virtual ~Layer() = default;

		virtual void OnAttach() = 0;
		virtual void OnDeattach() = 0;

		virtual void OnUpdate() = 0;
		virtual void OnEvent(const Event& e) = 0;
	};
}