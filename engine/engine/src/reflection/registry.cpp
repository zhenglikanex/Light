#include "engine/reflection/registry.h"
#include "engine/reflection/type.h"

namespace light::meta
{
	Registry* Registry::instance_ = nullptr;

	Registry& Registry::Get()
	{
		if(!instance_)
		{
			instance_ = new Registry();

			// 添加基础类型
			instance_->AddTypeData<bool>("bool");
			instance_->AddTypeData<char>("char");
			instance_->AddTypeData<unsigned char>("unsigned char");
			instance_->AddTypeData<short>("short");
			instance_->AddTypeData<unsigned short>("unsigned short");
			instance_->AddTypeData<int>("int");
			instance_->AddTypeData<unsigned int>("unsigned int");
			instance_->AddTypeData<long>("long");
			instance_->AddTypeData<unsigned long>("unsigned long");
			instance_->AddTypeData<long long>("long long");
			instance_->AddTypeData<unsigned long long>("unsigned long long");
			instance_->AddTypeData<float>("float");
			instance_->AddTypeData<double>("double");
			instance_->AddTypeData<std::string>("std::string");
		}

		return *instance_;
	}

	const TypeData* Registry::GetTypeData(const std::string& name) const
	{
		auto it = type_datas_.find(name);
		if(it != type_datas_.end())
		{
			return &it->second;
		}

		return nullptr;
	}

	const TypeData* Registry::GetTypeData(size_t type_id) const
	{
		auto it = type_data_by_id_map_.find(type_id);
		if(it != type_data_by_id_map_.end())
		{
			return it->second;
		}

		return nullptr;
	}
}

