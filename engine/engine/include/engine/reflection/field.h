#pragma once

#include <memory>
#include <string_view>
#include <string>
#include <any>

#include "engine/reflection/type.h"
#include "engine/reflection/field_wrapper.h"

namespace light::meta
{
	class Any;

	class Field
	{
	public:
		Field() = default;

		template<typename ... Properties>
		Field(std::string_view name,Type type,std::unique_ptr<FieldWrapperBase> field_wrapper, Properties&& ... properties)
			: name_(name)
			, type_(type)
			, field_wrapper_(std::move(field_wrapper))
		{
			if constexpr (sizeof...(Properties) > 0)
			{
				(properties_.emplace(typeid(Properties).hash_code(), std::forward<Properties>(properties)),...);
			}
		}

		bool IsValid() const { return type_.IsValid(); }

		bool SetValue(Any& instance,const Any& value) const;

		Any GetValue(Any& instance) const;

		std::string_view GetName() const { return name_; }

		Type GetType() const { return type_; }

		template<typename T>
		bool HasProperty()
		{
			return properties_.contains(typeid(T).hash_code());
		}

		template<typename T>
		const T* GetProperty() const
		{
			auto it = properties_.find(typeid(T).hash_code());
			if (it != properties_.end())
			{
				return &std::any_cast<const T&>(it->second);
			}
			return nullptr;
		}
	private:
		friend class TypeData;
		std::string name_;
		Type type_;
		std::unique_ptr<FieldWrapperBase> field_wrapper_;
		std::unordered_map<size_t, std::any> properties_;
	};
}