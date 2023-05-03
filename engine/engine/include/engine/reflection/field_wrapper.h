#pragma once

#include "engine/reflection/any.h"
#include "engine/reflection/type_traits.h"
#include "engine/log/log.h"

namespace light::meta
{
	class FieldWrapperBase
	{
	public:
		virtual ~FieldWrapperBase() = default;
		virtual bool SetValue(Any& instance, const Any& value) = 0;
		virtual Any GetValue(Any& instance) = 0;
		virtual Any GetRefValue(Any& instance) = 0;
	};

	template<typename ClassType, class FieldType>
	class FieldWrapper : public FieldWrapperBase
	{
	public:
		using FiledTypePtr = FieldType ClassType::*;

		FieldWrapper(FiledTypePtr field_ptr) : field_ptr_(field_ptr) {}

		bool SetValue(Any& instance, const Any& value) override
		{
			instance.Cast<ClassType>().*field_ptr_ = value.Cast<FieldType>();
			return true;
		}

		Any GetValue(Any& instance) override
		{
			if constexpr (details::IsVector<FieldType>::value)
			{
				return Any(std::ref(instance.Cast<ClassType>().*field_ptr_));
			}
			else
			{
				return Any(instance.Cast<ClassType>().*field_ptr_);
			}
		}

		Any GetRefValue(Any& instance) override
		{
			return Any(std::ref(instance.Cast<ClassType>().*field_ptr_));
		}
	private:
		FiledTypePtr field_ptr_;
	};
}

