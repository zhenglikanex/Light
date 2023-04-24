#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "engine/reflection/field.h"
#include "engine/reflection/method.h"

namespace light::meta
{
	class TypeData
	{
	public:
		TypeData();

		template<typename ClassType,typename FieldType,
			typename FieldTypePtr = typename FieldWapper<ClassType, FieldType>::FieldTypePtr>
		void AddField(std::string_view name, FieldTypePtr ptr)
		{
			fields_.emplace_back(name,(std::make_unique<FieldWapper<ClassType, FieldType>>(ptr)));
		}

		const Field& GetField(std::string_view name) const;
	private:
		std::string name_;
		std::vector<Field> fields_;
	};

	class Type
	{
	public:
		static const Type& Get(std::string_view name);

		std::string_view GetName() const;

		Field GetField(std::string_view name) const;
		Field GeMethod(std::string_view name) const;

	private:
		Type(std::string_view name);
		TypeData* data_;
	};
}