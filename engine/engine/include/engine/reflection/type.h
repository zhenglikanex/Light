#pragma once

#include <string_view>
#include <unordered_map>
#include <vector>

#include "engine/reflection/type_traits.h"
#include "engine/scene/entity.h"

namespace light::meta
{
	class Field;
	class Method;
	class TypeData;
	class Enum;
	class Any;

	class Type
	{
	public:
		template<typename T>
		static Type Get() requires (!details::IsVector<T>::value)
		{
			return Type(typeid(T).hash_code());
		}

		template<typename T>
		static Type Get() requires (details::IsVector<T>::value)
		{
			return Type(typeid(typename T::value_type).hash_code(),true);
		}


		Type() = default;
		explicit Type(const std::string& name, bool is_vector = false);
		explicit Type(size_t type_id, bool is_vector = false);

		Any AddComponent(Entity e) const;

		Any GetComponent(Entity e) const;

		bool HasComponent(Entity e) const;

		void RemoveComponent(Entity e) const;

		bool IsValid() const { return data_ != nullptr; }

		bool IsArray() const { return is_vector_; }

		bool IsEnum() const;

		std::string GetName() const;

		const Field& GetField(std::string_view name) const;

		const Method& GetMethod(std::string_view name) const;

		const std::vector<Field>& GetFields() const;

		const std::unordered_map<std::string,Method>& GetMethods() const;

		const std::vector<Enum>& GetEnumValues() const;

		bool operator==(const Type& other) const { return data_ == other.data_ && is_vector_ == other.is_vector_; }
		bool operator!=(const Type& other) const { return !(*this == other); }
	private:
		friend class Registry;

		const TypeData* data_ = nullptr;
		bool is_vector_ = false;
	};
}
