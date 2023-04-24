#include "engine/reflection/field.h"

namespace light::meta
{
	Field::Field(std::string_view name,std::unique_ptr<FieldWapperBase> field_wapper)
		: name_(name), field_wapper_(std::move(field_wapper))
	{

	}

	bool Field::SetValue(std::any& instance,const std::any& value) const
	{
		return field_wapper_->SetValue(instance,value);
	}

	std::any Field::GetValue(std::any& instance) const
	{
		return field_wapper_->GetValue(instance);
	}
}