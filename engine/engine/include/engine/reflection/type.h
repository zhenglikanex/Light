#pragma once

#include <string_view>

namespace light::meta
{
	class Field;
	class Method;
	class TypeData;

	class Type
	{
	public:
		template<typename T>
		static Type Get()
		{
			return Type(typeid(T).hash_code());
		}

		template<typename T>
		static Type Get(std::vector<T>)
		{
			return Type(typeid(T).hash_code(),true);
		}

		Type() = default;
		explicit Type(const std::string& name, bool is_vector = false);
		explicit Type(size_t type_id, bool is_vector = false);

		bool IsValid() const { return data_ != nullptr; }

		bool IsArray() const { return is_vector_; }

		std::string_view GetName() const;

		const Field& GetField(std::string_view name) const;

		const Method& GeMethod(std::string_view name) const;		
	private:
		friend class Registry;

		const TypeData* data_;
		bool is_vector_ = false;
	};
}
