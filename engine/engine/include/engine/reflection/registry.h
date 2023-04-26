#pragma once

#include "engine/reflection/type_data.h"
#include "engine/reflection/type.h"

namespace light::meta
{
	class Registry
	{
	public:
		static Registry& Get();

		template<typename T>
		TypeData& AddTypeData(std::string_view name)
		{
			auto& type_data = types_.emplace(name, TypeData(name)).first->second;
			type_by_id_map_.emplace(typeid(T).hash_code(), &type_data);
			return type_data;
		}

		template<typename T>
		Type GetType()
		{
			auto it = type_by_id_map_.find(typeid(T).hash_code());
			if (it != type_by_id_map_.end())
			{
				return Type(it->second);
			}

			return Type(nullptr);
		}

		Type GetType(std::string_view name);
	private:
		static Registry instance_;

		std::unordered_map<std::string, TypeData> types_;
		std::unordered_map<size_t, TypeData*> type_by_id_map_;
	};
}