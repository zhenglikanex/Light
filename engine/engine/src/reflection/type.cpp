#include "engine/reflection/type.h"
#include "engine/reflection/field.h"
#include "engine/reflection/method.h"
#include "engine/reflection/registry.h"

namespace light::meta
{
	Type light::meta::Type::Get(std::string_view name)
	{
		return Registry::Get().GetType(name);
	}

	std::string_view light::meta::Type::GetName() const
	{
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

	Type::Type(const TypeData* data)
		: data_(data)
	{
		
	}
}
