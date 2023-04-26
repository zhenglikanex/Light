#include "engine/reflection/type_data.h"

namespace light::meta
{
	const Field& TypeData::GetField(std::string_view name) const
	{
		auto it = std::find_if(fields_.begin(), fields_.end(), [name](const Field& field) { return field.GetName() == name; });
		if (it != fields_.end())
		{
			return *it;
		}

		static Field null_field("", nullptr);
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

	TypeData::TypeData(std::string_view name)
		: name_(name)
	{

	}
}