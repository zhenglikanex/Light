#pragma once

#include <type_traits>
#include "engine/reflection/any.h"

namespace light::meta
{
	using MethodInvokeArgs = std::vector<Any>;

	class MethodInvokeBase
	{
	public:
		using MethodInvokeFunc = Any(*)(MethodInvokeBase*, Any&, const MethodInvokeArgs& args);

		Any Invoke(Any& instance, const MethodInvokeArgs& args);
	protected:
		MethodInvokeFunc invoke_impl_ = nullptr;
	};

	template<bool IsConstFunc,typename ClassType, typename ReturnType, typename ... Args>
	class MethodInvoke : public MethodInvokeBase
	{
	public:
		using MethodType =  std::conditional_t<IsConstFunc,
			ReturnType(ClassType::*)(Args...) const,
			ReturnType(ClassType::*)(Args...)> ;

		MethodInvoke(MethodType method) : method_(method) {
			invoke_impl_ = &MethodInvoke<IsConstFunc, ClassType, ReturnType, Args...>::InvokeImpl;
		}

		static void Test()
		{
			method_ = nullptr;
		}

		
		static Any InvokeImpl(MethodInvokeBase* invoke_base, Any& instance, const MethodInvokeArgs& args)
		{
			auto invoke = static_cast<MethodInvoke<IsConstFunc,ClassType, ReturnType, Args...>*>(invoke_base);

			if constexpr (sizeof...(Args) > 0)
			{
				return invoke->template InvokeImpl<Args...>(instance, args);
			}
			else
			{
				return invoke->InvokeNoArgs(instance);
			}
		}
	private:
		template<typename T1>
		Any InvokeImpl(Any& instance, const MethodInvokeArgs& args)
		{
			if constexpr (std::is_void_v<ReturnType>)
			{
				auto& inst = instance.Cast<ClassType>();
				(inst.*method_)(args[0].Cast<T1>());
				return Any{};
			}
			else
			{
				if constexpr (std::is_reference_v<ReturnType>)
				{
					if constexpr (std::is_const_v<std::remove_reference_t<ReturnType>>)
					{
						return std::cref((instance.Cast<ClassType>().*method_)(args[0].Cast<T1>()));
					}
					else
					{
						return std::ref((instance.Cast<ClassType>().*method_)(args[0].Cast<T1>()));
					}
				}
				else
				{
					return (instance.Cast<ClassType>().*method_)(args[0].Cast<T1>());
				}
			}
		}


		template<typename T1,typename T2>
		Any InvokeImpl(Any& instance, const MethodInvokeArgs& args)
		{
			if constexpr (std::is_void_v<ReturnType>)
			{
				auto& inst = instance.Cast<ClassType>();
				(inst.*method_)(args[0].Cast<T1>(), args[0].Cast<T2>());
				return Any{};
			}
			else
			{
				if constexpr (std::is_reference_v<ReturnType>)
				{
					if constexpr (std::is_const_v<std::remove_reference_t<ReturnType>>)
					{
						return std::cref((instance.Cast<ClassType>().*method_)(args[0].Cast<T1>(), args[0].Cast<T2>()));
					}
					else
					{
						return std::ref((instance.Cast<ClassType>().*method_)(args[0].Cast<T1>(), args[0].Cast<T2>()));
					}
				}
				else
				{
					return (instance.Cast<ClassType>().*method_)(args[0].Cast<T1>(), args[0].Cast<T2>());
				}
			}
		}

		template<typename T1,typename T2,typename T3>
		Any InvokeImpl(Any& instance, const MethodInvokeArgs& args)
		{
			if constexpr (std::is_void_v<ReturnType>)
			{
				auto& inst = instance.Cast<ClassType>();
				(inst.*method_)(args[0].Cast<T1>(), args[0].Cast<T2>(), args[0].Cast<T3>());
				return Any{};
			}
			else
			{
				if constexpr (std::is_reference_v<ReturnType>)
				{
					if constexpr (std::is_const_v<std::remove_reference_t<ReturnType>>)
					{
						return std::cref((instance.Cast<ClassType>().*method_)(args[0].Cast<T1>(), args[0].Cast<T2>(), args[0].Cast<T3>()));
					}
					else
					{
						return std::ref((instance.Cast<ClassType>().*method_)(args[0].Cast<T1>(), args[0].Cast<T2>(),args[0].Cast<T3>()));
					}
				}
				else
				{
					return (instance.Cast<ClassType>().*method_)(args[0].Cast<T1>(),args[0].Cast<T2>(), args[0].Cast<T3>());
				}
			}
		}

		template<typename T1, typename T2, typename T3,typename T4>
		Any InvokeImpl(Any& instance, const MethodInvokeArgs& args)
		{
			if constexpr (std::is_void_v<ReturnType>)
			{
				auto& inst = instance.Cast<ClassType>();
				(inst.*method_)(args[0].Cast<T1>(), args[1].Cast<T2>(), args[2].Cast<T3>(), args[3].Cast<T4>());
				return Any{};
			}
			else
			{
				if constexpr (std::is_reference_v<ReturnType>)
				{
					if constexpr (std::is_const_v<std::remove_reference_t<ReturnType>>)
					{
						return std::cref((instance.Cast<ClassType>().*method_)(args[0].Cast<T1>(), args[1].Cast<T2>(), args[2].Cast<T3>(), args[3].Cast<T4>()));
					}
					else
					{
						return std::ref((instance.Cast<ClassType>().*method_)(args[0].Cast<T1>(), args[1].Cast<T2>(), args[2].Cast<T3>(), args[3].Cast<T4>()));
					}
				}
				else
				{
					return (instance.Cast<ClassType>().*method_)(args[0].Cast<T1>(), args[1].Cast<T2>(), args[2].Cast<T3>(), args[3].Cast<T4>());
				}
			}
		}

		template<typename T1, typename T2, typename T3, typename T4,typename T5>
		Any InvokeImpl(Any& instance, const MethodInvokeArgs& args)
		{
			if constexpr (std::is_void_v<ReturnType>)
			{
				auto& inst = instance.Cast<ClassType>();
				(inst.*method_)(args[0].Cast<T1>(), args[1].Cast<T2>(), args[2].Cast<T3>(), args[3].Cast<T4>(), args[4].Cast<T5>());
				return Any{};
			}
			else
			{
				if constexpr (std::is_reference_v<ReturnType>)
				{
					if constexpr (std::is_const_v<std::remove_reference_t<ReturnType>>)
					{
						return std::cref((instance.Cast<ClassType>().*method_)(args[0].Cast<T1>(), args[1].Cast<T2>(), args[2].Cast<T3>(), args[3].Cast<T4>(), args[4].Cast<T5>()));
					}
					else
					{
						return std::ref((instance.Cast<ClassType>().*method_)(args[0].Cast<T1>(), args[1].Cast<T2>(), args[2].Cast<T3>(), args[3].Cast<T4>(), args[4].Cast<T5>()));
					}
				}
				else
				{
					return (instance.Cast<ClassType>().*method_)(args[0].Cast<T1>(), args[1].Cast<T2>(), args[2].Cast<T3>(), args[3].Cast<T4>(), args[4].Cast<T5>());
				}
			}
		}
		
		Any InvokeNoArgs(Any& instance)
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

		MethodType method_;
	};
}