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

	bool Type::IsEnum() const
	{
		if (!IsValid())
		{
			return false;
		}

		return data_->IsEnum();
	}

	std::string Type::GetName() const
	{
		if (!IsValid())
		{
			return "unknown type";
		}

		if(is_vector_)
		{
			return std::format("std::vector<{}>", data_->GetName());
		}
		return std::string(data_->GetName());
	}

	const Field& Type::GetField(std::string_view name) const
	{
		return data_->GetField(name);
	}

	const Method& Type::GetMethod(std::string_view name) const
	{
		return data_->GetMethod(name);
	}

	const std::vector<Field>& Type::GetFields() const
	{
		return data_->GetFields();
	}

	const std::unordered_map<std::string, Method>& Type::GetMethods() const
	{
		return data_->GetMethods();
	}

	const std::vector<Enum>& Type::GetEnumValues() const
	{
		return data_->GetEnumValues();
	}
}
