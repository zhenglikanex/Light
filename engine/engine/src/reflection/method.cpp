#include "engine/reflection/method.h"

namespace light::meta
{
	Any MethodInvokeBase::Invoke(Any& instance, const std::any& args)
	{
		return invoke_impl_(this, instance, args);
	}

	Method::Method(std::string_view name, std::unique_ptr<MethodInvokeBase> invokable)
		: name_(name)
		, invokable_(std::move(invokable))
	{
		
	}

}

