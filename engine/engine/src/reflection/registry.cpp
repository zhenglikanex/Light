#include "engine/reflection/registry.h"

namespace light::meta
{
	Registry Registry::instance_;

	Registry& Registry::Get()
	{
		return instance_;
	}

	Type Registry::GetType(std::string_view name)
	{
		auto it = types_.find(std::string(name));
		if (it != types_.end())
		{
			return Type(&it->second);
		}
		return Type(nullptr);
	}
}

