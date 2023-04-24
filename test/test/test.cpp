#include <typeindex>
#include <iostream>
#include <typeindex>
#include "engine/reflection/type.h"
#include "engine/reflection/any.h"

using namespace light;
using namespace meta;

struct Test
{
	int a = 0;
	int b = 2;
	int c = 3;
};

template<typename ClassType, class FieldType>
class T
{
public:
	using FiledTypePtr = FieldType ClassType::*;
};

class Big
{
public:
	Big() = default;
	Big(const Big& other)
	{
		std::cout << "Big(const Big& other)" << std::endl;
	}

	Big(Big&& other) noexcept
	{
		std::cout << "Big" << std::endl;
	}

	Big& operator=(const Big& other)
	{
		std::cout << "Big& operator=(const Big& other)" << std::endl;

		return *this;
	}

	Big& operator=(Big&& other)
	{
		std::cout << "Big& operator=(Big&& other)" << std::endl;
		return *this;
	}

	~Big()
	{
		std::cout << "~Big" << std::endl;
	}

	std::vector<int> a;
	std::vector<int> b;
	std::vector<int> c;
};

int main()
{
	auto tst = kSmallObjectFuncCollection<std::string>;

	Any a(10);
	int va = a.Cast<int>();

	std::string str = "niubi";
	Any astr(str);

	astr.Reset();

	Any big(Big{});
	big.Reset();

	Any copybig = big;

	Any copy2(big);
	

	
	Test t;
	TypeData data;
	
	data.AddField<Test, int>("a",&Test::a);
	
	auto& f = data.GetField("a");
	
	std::any instance = t;
	f.SetValue(instance, 10);
	std::any v = f.GetValue(instance);
	auto iv = std::any_cast<int>(v);
	
	Test& t2 = std::any_cast<Test&>(instance);
	return 0;
}