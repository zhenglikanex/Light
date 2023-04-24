#pragma once

#include <any>
#include <cstdint>
#include <memory>
#include <string_view>
#include <string>

namespace light::meta
{
	class FieldWapperBase
	{
	public:
		virtual bool SetValue(std::any& instance,const std::any& value) = 0;
		virtual std::any GetValue(std::any& instance) = 0;
	};

	template<typename ClassType,class FieldType>
	class FieldWapper : public FieldWapperBase
	{
	public:
		using FiledTypePtr = FieldType ClassType::*;

		FieldWapper(FiledTypePtr field_ptr) : field_ptr_(field_ptr) {}

		bool SetValue(std::any& instance,const std::any& value) override
		{
			std::any_cast<ClassType&>(instance).*field_ptr_ = std::any_cast<FieldType>(value);
			return true;
		}

		std::any GetValue(std::any& instance) override
		{
			return std::make_any<FieldType>(std::any_cast<ClassType&>(instance).*field_ptr_);
		}
	private:
		FiledTypePtr field_ptr_;
	};

	class Field
	{
	public:
		Field(std::string_view name,std::unique_ptr<FieldWapperBase> field_wapper);

		bool SetValue(std::any& instance,const std::any& value) const;
		std::any GetValue(std::any& instance) const;

		std::string_view GetName() const { return name_; }
	private:
		std::string name_;
		std::unique_ptr<FieldWapperBase> field_wapper_;
	};
}