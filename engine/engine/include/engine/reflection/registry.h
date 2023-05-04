#pragma once

#include "engine/reflection/type_data.h"
#include "engine/reflection/any.h"
#include "engine/reflection/type.h"

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
				if constexpr (std::is_empty_v<T>)
				{
					return Any();
				}
				else
				{
					return Any(std::ref(entity.GetComponent<T>()));
				}
			};

			type_data.add_component_func_ = [](Entity entity)
			{
				if constexpr (std::is_empty_v<T>)
				{
					return Any();
				}
				else
				{
					return Any(std::ref(entity.AddComponent<T>()));
				}
			};

			type_data.remove_component_func_ = [](Entity entity)
			{
				entity.RemoveComponent<T>();
			};

			return type_data;
		}

		const TypeData* GetTypeData(const std::string& name) const;

		const TypeData* GetTypeData(size_t type_id) const;

		// 查找所有T类型的子类
		template<typename T>
		std::vector<Type> FindSubTypes()
		{
			const TypeData* target_type_data = GetTypeData(typeid(T).hash_code());
			if (target_type_data == nullptr)
			{
				return {};
			}

			std::vector<Type> types;

			for (auto& [name, type_data] : type_datas_)
			{
				if (type_data.IsSubTypeOf(target_type_data->GetName()))
				{
					types.emplace_back(name,false);
				}
			}
			return types;
		}
	private:
		static Registry* instance_;

		std::unordered_map<std::string, TypeData> type_datas_;
		std::unordered_map<size_t, TypeData*> type_data_by_id_map_;
	};
}