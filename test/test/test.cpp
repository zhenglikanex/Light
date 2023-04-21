#include <typeindex>
#include <iostream>

#include <rttr/registration.h>
#include <rttr/type>

int main()
{
	rttr::type t = rttr::type::get_by_name("std::vector<int>");
	auto v = t.create();
	std::cout << t.get_name().to_string() << std::endl;
	return 0;
}