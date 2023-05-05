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
		Field(std::string_view name,size_t type_id,bool is_vector,std::unique_ptr<FieldWrapperBase> field_wrapper, Properties&& ... properties)
			: name_(name)
			, type_id_(type_id)
			, is_vector_(is_vector)
			, field_wrapper_(std::move(field_wrapper))
		{
			if constexpr (sizeof...(Properties) > 0)
			{
				(properties_.emplace(typeid(Properties).hash_code(), std::forward<Properties>(properties)),...);
			}
		}

		bool IsValid() const { return GetType().IsValid(); }

		bool SetValue(Any& instance,const Any& value) const;

		Any GetValue(Any& instance) const;

		Any GetRefValue(Any& instance) const;

		std::string_view GetName() const { return name_; }

		Type GetType() const { return Type(type_id_,is_vector_); }

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
		size_t type_id_;
		bool is_vector_;
		std::unique_ptr<FieldWrapperBase> field_wrapper_;
		std::unordered_map<size_t, std::any> properties_;
	};
}