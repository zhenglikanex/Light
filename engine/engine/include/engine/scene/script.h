#pragma once
#include "light_pch.h"

#include "engine/scene/entity.h"
#include "engine/core/timestep.h"

namespace light
{
	class Script
	{
	public:
		Entity entity;

		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(Timestep ts) {};
	};
}