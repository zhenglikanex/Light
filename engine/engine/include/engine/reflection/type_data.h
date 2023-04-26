#pragma once

#include "engine/reflection/field.h"
#include "engine/reflection/method.h"

namespace light::meta
{
	class TypeData
	{
	public:
		TypeData(std::string_view name);
		TypeData(const TypeData&) = default;
		TypeData(TypeData&&) = default;
		TypeData& operator=(const TypeData&) = default;
		TypeData& operator=(TypeData&&) = default;

		~TypeData() = default;

		template<typename ClassType, typename FieldType,
			typename FieldTypePtr = typename FieldWapper<ClassType, FieldType>::FieldTypePtr,
			typename ... Propertys>
		void AddField(std::string_view name, FieldTypePtr ptr,Propertys&& ... propertys)
		{
			fields_.emplace_back(name, std::make_unique<FieldWapper<ClassType, FieldType>>(ptr),std::forward<Propertys>(propertys)...);
		}

		template<typename ClassType,typename RetunrType,typename ... Args>
		void AddMethod(std::string_view name, RetunrType(ClassType::* method)(Args...))
		{
			methods_.emplace(name, Method(name,std::make_unique<MethodInvoke<ClassType,RetunrType,Args...>>(method)));
		}

		std::string_view GetName() const { return name_; }

		const Field& GetField(std::string_view name) const;

		const Method& GetMethod(std::string_view name) const;
	private:
		friend class Registry;

		std::string name_;
		std::vector<Field> fields_;
		std::unordered_map<std::string, Method> methods_;
	};
}