#pragma once

#include "engine/reflection/field.h"
#include "engine/reflection/method.h"
#include "engine/reflection/enum.h"

namespace light::meta
{
	class TypeData
	{
	public:
		TypeData(std::string_view name,bool is_enum);
		TypeData(const TypeData&) = default;
		TypeData(TypeData&&) = default;
		TypeData& operator=(const TypeData&) = default;
		TypeData& operator=(TypeData&&) = default;

		~TypeData() = default;

		template<typename ClassType, typename FieldType,
			typename FieldTypePtr = typename FieldWrapper<ClassType, FieldType>::FieldTypePtr,
			typename ... Properties>
		void AddField(std::string_view name, FieldTypePtr ptr, Properties&& ... properties)
		{
			fields_.emplace_back(name,Type::Get<FieldType>(),std::make_unique<FieldWrapper<ClassType, FieldType>>(ptr),std::forward<Properties>(properties)...);
		}

		template<typename ClassType,typename ReturnType,typename ... Args>
		void AddMethod(std::string_view name, ReturnType(ClassType::* method)(Args...))
		{
			methods_.emplace(name, Method(name,std::make_unique<MethodInvoke<ClassType, ReturnType,Args...>>(method)));
		}

		void AddEnum(std::string_view name, int64_t value);

		std::string_view GetName() const { return name_; }

		bool IsEnum() const { return is_enum_; }

		const Field& GetField(std::string_view name) const;

		const Method& GetMethod(std::string_view name) const;

		const Enum& GetEnum(std::string_view name) const;

		const std::vector<Field>& GetFields() const { return fields_; }
		
		const std::unordered_map<std::string, Method>& GetMethods() const { return methods_; }

		const std::vector<Enum>& GetEnumValues() const { return enum_values_; }
	private:
		friend class Registry;

		std::string name_;
		bool is_enum_ = false;
		std::vector<Field> fields_;
		std::unordered_map<std::string, Method> methods_;
		std::vector<Enum> enum_values_;
	};
}