#pragma once

#include <any>
#include "engine/reflection/type.h"
#include "engine/reflection/any.h"

namespace light::meta
{
	class MethodInvokeBase
	{
	public:
		using MethodInvokeFunc = Any(*)(MethodInvokeBase*,Any&,const std::any& args);

		Any Invoke(Any& instance, const std::any& args);
	protected:
		MethodInvokeFunc invoke_impl_;
	};

	template<typename ClassType,typename ReturnType,typename ... Args>
	class MethodInvoke : public MethodInvokeBase
	{
	public:
		using MethodType = ReturnType(ClassType::*)(Args...);

		MethodInvoke(MethodType method) : method_(method) {
			invoke_impl_ = &MethodInvoke::InvokeImpl;
		}

		static Any InvokeImpl(MethodInvokeBase* invoke_base,Any& instance, const std::any& args)
		{	
			auto invoke = (MethodInvoke<ClassType,ReturnType, Args...>*)invoke_base;

			if constexpr (sizeof...(Args) > 0)
			{
				auto tuple_args = std::any_cast<const std::tuple<Args...>&>(args);
				return invoke->InvokeForTuple(instance, tuple_args, std::index_sequence_for<Args...>{});
			}
			else
			{
				auto& inst = instance.Cast<ClassType>();
				if constexpr (std::is_void_v<ReturnType>)
				{
					(inst.*method_)();
					return Any{};
				}
				else
				{
					if constexpr (std::is_reference_v<ReturnType>)
					{
						if constexpr (std::is_const_v<ReturnType>)
						{
							return std::cref((inst.*method_)());
						}
						else
						{
							return std::ref((inst.*method_)());
						}
					}
					else 
					{
						return (inst.*method_)();
					}
				}
				
			}
		}
	private:
		template<std::size_t ... Indexs>
		Any InvokeForTuple(Any& instance,const std::tuple<Args...>& tuple_args,std::index_sequence<Indexs...>)
		{
			if constexpr (std::is_void_v<ReturnType>)
			{
				auto& inst = instance.Cast<ClassType>();
				(inst.*method_)(std::get<Indexs>(tuple_args)...);
				return Any{};
			}
			else 
			{
				if constexpr (std::is_reference_v<ReturnType>)
				{
					if constexpr (std::is_const_v<std::remove_reference_t<ReturnType>>)
					{
						return std::cref((instance.Cast<ClassType>().*method_)(std::get<Indexs>(tuple_args)...));
					}
					else
					{
						return std::ref((instance.Cast<ClassType>().*method_)(std::get<Indexs>(tuple_args)...));
					}
				}
				else
				{
					return (instance.Cast<ClassType>().*method_)(std::get<Indexs>(tuple_args)...);
				}
			}
			
		}

		MethodType method_;
	};

	class Method
	{
	public:
		Method() = default;
		Method(std::string_view name,std::unique_ptr<MethodInvokeBase> invokable);

		template<class ...Args>
		Any Invoke(Any& instance, Args&& ... args) const
		{
			return invokable_->Invoke(instance, std::tuple<Args...>{ args... });
		}
		
		std::string_view GetName() const { return name_; }
	private:
		std::string name_;
		std::unique_ptr<MethodInvokeBase> invokable_;
	};
}