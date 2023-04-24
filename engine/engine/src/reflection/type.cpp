#include "engine/reflection/type.h"

namespace light::meta
{
	TypeData::TypeData()
	{

	}

	const Field& TypeData::GetField(std::string_view name) const
	{
		auto it = std::find_if(fields_.begin(), fields_.end(), [name](const Field& field) { return field.GetName() == name; });
		if (it != fields_.end())
		{
			return *it;
		}

		static Field null_field("",nullptr);
		return null_field;
	}
}