#include "engine/reflection/any.h"
#include "engine/reflection/type.h"

namespace light::meta
{
	Type Any::GetType() const
	{
		return Type(data_.type_id, data_.is_vector);
	}

	size_t Any::GetSize() const
	{
		LIGHT_ASSERT(GetType().IsArray(), "any is not array");

		if (data_.is_ref)
		{
			return data_.vector_ref_data.object_func_collection->vector_size_func(data_.vector_ref_data.data);
		}
		else
		{
			if (data_.object_type == AnyValueObjectType::kSmall)
			{
				return data_.small_object_data.object_func_collection->vector_size_func(data_.small_object_data.data);
			}
			else
			{
				return data_.big_object_data.object_func_collection->vector_size_func(data_.big_object_data.ptr);
			}
		}
	}

	Any Any::GetElement(size_t index)
	{
		LIGHT_ASSERT(GetType().IsArray(), "any is not array");

		if (data_.is_ref)
		{
			return data_.vector_ref_data.object_func_collection->get_element_func(data_.vector_ref_data.data, index);
		}
		else
		{
			if (data_.object_type == AnyValueObjectType::kSmall)
			{
				return data_.small_object_data.object_func_collection->get_element_func(data_.small_object_data.data, index);
			}
			else
			{
				return data_.big_object_data.object_func_collection->get_element_func(data_.big_object_data.ptr, index);
			}
		}
	}
}