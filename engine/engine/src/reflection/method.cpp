#include "engine/reflection/method.h"

namespace light::meta
{
	Method::Method(std::string_view name, std::unique_ptr<MethodInvokeBase> invokable)
		: name_(name)
		, invokable_(std::move(invokable))
	{
		
	}

}

