#include "engine/reflection/field.h"

namespace light::meta
{

	bool Field::SetValue(Any& instance,const Any& value) const
	{
		return field_wapper_->SetValue(instance,value);
	}

	Any Field::GetValue(Any& instance) const
	{
		return field_wapper_->GetValue(instance);
	}
}