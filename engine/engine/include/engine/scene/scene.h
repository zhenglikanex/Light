#pragma once

#include "entt/entt.hpp"

namespace light
{
	class Scene
	{
	public:
		Scene();
	private:
		entt::registry registry_;
	};
}
