#pragma once

#include "engine/reflection/type.h"

namespace light::meta
{
	class Registry
	{
	public:
		Type GetType(std::string_view name);
	private:
	};
}