#include "engine/reflection/type_data.h"

#include "engine/core/core.h"

namespace light::meta
{
	TypeData::TypeData(std::string_view name, bool is_enum)
		: name_(name)
		, is_enum_(is_enum)
	{
	}

	void TypeData::AddEnum(std::string_view name, int64_t value)
	{
		LIGHT_ASSERT(is_enum_, std::format("{} is not a enum!", name_));

		enum_values_.emplace_back(name, value);
	}

	const Field& TypeData::GetField(std::string_view name) const
	{
		auto it = std::ranges::find_if(fields_, [name](const Field& field) { return field.GetName() == name; });
		if (it != fields_.end())
		{
			return *it;
		}

		static Field null_field;
		return null_field;
	}

	const Method& TypeData::GetMethod(std::string_view name) const
	{
		auto it = methods_.find(std::string(name));
		if (it != methods_.end())
		{
			return it->second;
		}

		static Method null_method;
		return null_method;
	}

	const Enum& TypeData::GetEnum(std::string_view name) const
	{
		auto it = std::ranges::find_if(enum_values_, [name](const Enum& e) { return e.GetName() == name; });
		if (it != enum_values_.end())
		{
			return *it;
		}

		static Enum null_enum;
		return null_enum;
	}

}