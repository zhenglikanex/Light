#include "engine/reflection/field.h"
#include "engine/reflection/field_wrapper.h"

namespace light::meta
{
	bool Field::SetValue(Any& instance,const Any& value) const
	{
		return field_wrapper_->SetValue(instance,value);
	}

	Any Field::GetValue(Any& instance) const
	{
		return field_wrapper_->GetValue(instance);
	}

	Any Field::GetRefValue(Any& instance) const
	{
		return field_wrapper_->GetRefValue(instance);
	}
}
