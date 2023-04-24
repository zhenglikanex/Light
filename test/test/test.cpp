#include <typeindex>
#include <iostream>

#include "engine/reflection/type.h"

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

int main()
{
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