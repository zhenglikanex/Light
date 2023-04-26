#include "engine/reflection/method_invoke.h"

namespace light::meta
{
	Any MethodInvokeBase::Invoke(Any& instance, const std::any& args)
	{
		return invoke_impl_(this, instance, args);
	}
}
