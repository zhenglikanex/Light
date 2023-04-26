#include <format>

#include "engine/reflection/type.h"
#include "engine/reflection/field.h"
#include "engine/reflection/method.h"
#include "engine/reflection/registry.h"

namespace light::meta
{
	Type::Type(const std::string& name, bool is_vector)
		: data_(Registry::Get().GetTypeData(name))
		, is_vector_(is_vector)
	{
	}

	Type::Type(size_t type_id, bool is_vector)
		: data_(Registry::Get().GetTypeData(type_id))
		, is_vector_(is_vector)
	{
	}

	std::string_view Type::GetName() const
	{
		if(is_vector_)
		{
			return std::format("std::vector<{}>", data_->GetName());
		}
		return data_->GetName();
	}

	const Field& Type::GetField(std::string_view name) const
	{
		return data_->GetField(name);
	}

	const Method& Type::GeMethod(std::string_view name) const
	{
		return data_->GetMethod(name);
	}
}
