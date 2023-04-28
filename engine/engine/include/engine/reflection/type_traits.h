#pragma once

#include <vector>

namespace light::meta::details
{
	template<typename T>
	struct IsVector
	{
		constexpr static bool value = false;
	};

	template<typename T>
	struct IsVector<std::vector<T>>
	{
		constexpr static bool value = true;
	};
}