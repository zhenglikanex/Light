#pragma once

#include <cstdint>
#include <memory>
#include <string_view>
#include <string>
#include <any>


#include "engine/reflection/any.h"

namespace light::meta
{
	
	class FieldWapperBase
	{
	public:
		virtual bool SetValue(Any& instance,const Any& value) = 0;
		virtual Any GetValue(Any& instance) = 0;
	};

	template<typename ClassType,class FieldType>
	class FieldWapper : public FieldWapperBase
	{
	public:
		using FiledTypePtr = FieldType ClassType::*;

		FieldWapper(FiledTypePtr field_ptr) : field_ptr_(field_ptr) {}

		bool SetValue(Any& instance,const Any& value) override
		{
			instance.Cast<ClassType>().*field_ptr_ = value.Cast<FieldType>();
			return true;
		}

		Any GetValue(Any& instance) override
		{
			return Any(instance.Cast<ClassType>().*field_ptr_);
		}
	private:
		FiledTypePtr field_ptr_;
	};

	class Field
	{
	public:
		template<typename ... Propertys>
		Field(std::string_view name, std::unique_ptr<FieldWapperBase> field_wapper, Propertys&& ... propertys)
			: name_(name), field_wapper_(std::move(field_wapper))
		{
			if constexpr (sizeof...(Propertys) > 0)
			{
				(propertys_.emplace(typeid(Propertys).hash_code(), std::forward<Propertys>(propertys)),...);
			}
		}

		bool SetValue(Any& instance,const Any& value) const;
		Any GetValue(Any& instance) const;

		std::string_view GetName() const { return name_; }

		template<typename T>
		bool HasProperty()
		{
			return propertys_.contains(typeid(T).hash_code());
		}

		template<typename T>
		const T* GetProperty() const
		{
			auto it = propertys_.find(typeid(T).hash_code());
			if (it != propertys_.end())
			{
				return &std::any_cast<const T&>(it->second);
			}
			return nullptr;
		}
	private:
		friend class TypeData;

		std::string name_;
		std::unique_ptr<FieldWapperBase> field_wapper_;
		std::unordered_map<size_t, std::any> propertys_;
	};
}