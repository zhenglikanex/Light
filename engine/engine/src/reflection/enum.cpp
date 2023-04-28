#include "engine/reflection/enum.h"

namespace light::meta
{
	Enum::Enum(std::string_view name, uint64_t value)
		: name_(name)
		, value_(value)
	{
	}
}

