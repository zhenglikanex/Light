#pragma once

#include "engine/reflection/any.h"
#include "engine/reflection/method_invoke.h"

namespace light::meta
{
	class Any;
	class MethodInvokeBase;
	class Method
	{
	public:
		Method() = default;
		Method(std::string_view name,std::unique_ptr<MethodInvokeBase> invokable);

		template<class ...Args>
		Any Invoke(Any& instance, Args&& ... args) const
		{
			return invokable_->Invoke(instance, MethodInvokeArgs{ Any(args)... });
		}

		std::string_view GetName() const { return name_; }

		bool IsValid() const { return !name_.empty(); }
	private:
		std::string name_;
		std::unique_ptr<MethodInvokeBase> invokable_;
	};
}