#pragma once

#include "engine/reflection/type_data.h"

namespace light::meta
{
	class Type;

	class Registry
	{
	public:
		static Registry& Get();

		template<typename T>
		TypeData& AddTypeData(std::string_view name)
		{
			auto& type_data = type_datas_.emplace(name, TypeData(name,std::is_enum_v<T>)).first->second;
			type_data_by_id_map_.emplace(typeid(T).hash_code(), &type_data);

			type_data.has_component_func_ = [](Entity entity)
			{
				return entity.HasComponent<T>();
			};

			type_data.get_component_func_ = [](Entity entity)
			{
				return Any(std::ref(entity.GetComponent<T>()));
			};

			type_data.add_component_func_ = [](Entity entity)
			{
				entity.AddComponent<T>();
			};

			type_data.remove_component_func_ = [](Entity entity)
			{
								entity.RemoveComponent<T>();
			};

			return type_data;
		}

		const TypeData* GetTypeData(const std::string& name) const;

		const TypeData* GetTypeData(size_t type_id) const;
	private:
		static Registry* instance_;

		std::unordered_map<std::string, TypeData> type_datas_;
		std::unordered_map<size_t, TypeData*> type_data_by_id_map_;
	};
}