#include <format>

#include "engine/core/core.h"
#include "engine/reflection/type.h"
#include "engine/reflection/field.h"
#include "engine/reflection/method.h"
#include "engine/reflection/registry.h"
#include "engine/reflection/any.h"

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

	Any Type::AddComponent(Entity e) const
	{
		LIGHT_ASSERT(IsValid(), "is unkown type!");
		return data_->add_component_func_(e);
	}

	Any Type::GetComponent(Entity e) const
	{
		LIGHT_ASSERT(IsValid(), "is unkown type!");
		return data_->get_component_func_(e);
	}

	bool Type::HasComponent(Entity e) const
	{
		LIGHT_ASSERT(IsValid(), "is unkown type!");
		return data_->has_component_func_(e);
	}

	void Type::RemoveComponent(Entity e) const
	{
		LIGHT_ASSERT(IsValid(), "is unkown type!");
		data_->remove_component_func_(e);
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
