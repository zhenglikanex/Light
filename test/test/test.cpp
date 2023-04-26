#include <typeindex>
#include <iostream>
#include <typeindex>
#include "engine/reflection/type_data.h"
#include "engine/reflection/type.h"
#include "engine/reflection/any.h"
#include "engine/reflection/registry.h"
#include "engine/reflection/method_invoke.h"

using namespace light;
using namespace meta;

struct Test
{
	void Print(int& a,int& b)
	{
		std::cout << "print" << a << std::endl;
		a += 10;
		b + 20;
	}

	int TestReturn(int a,int b)
	{
		return 1000;
	}

	const int& TestReturnRef(int _a,int _b,int _c)
	{
		a = _a + _b + _c;
		return a;
	}

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
		std::cout << "Move Big(Big&& other)" << std::endl;
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

class Small
{
public:
	Small() = default;

	~Small()
	{
		std::cout << "~Small()" << std::endl;
	}

	Small(const Small&)
	{
		std::cout << "Small(const Small&)" << std::endl;
	}

	Small(Small&&)
	{
		std::cout << "Move Small" << std::endl;
	}



private:
	int a;
	int b;
	int c;
};

void TestAny()
{
	// big
	{
		Big value;
		Any big(value);
		// cout << copy 

		Any copy_big = big;
		// cout << copy

		Any move_big = std::move(big);
		// cout << move

		copy_big = move_big;
		// cout << ~big
		// cout << copy

		copy_big = std::move(move_big);
		// cout << ~big
		// cout << move
		
		std::cout << "=====================================" << std::endl;
	}

	// small
	{
		Small value;
		Any big(value);
		// cout << copy 

		Any copy_big = big;
		// cout << copy

		Any move_big = std::move(big);
		// cout << move

		copy_big = move_big;
		// cout << ~big
		// cout << copy

		copy_big = std::move(move_big);
		// cout << ~big
		// cout << move
	}

	// trivial
	{
		int value = 10;
		Any big(value);
		// cout << copy 

		Any copy_big = big;
		// cout << copy

		Any move_big = std::move(big);
		// cout << move

		copy_big = move_big;
		// cout << ~big
		// cout << copy

		copy_big = std::move(move_big);
		// cout << ~big
		// cout << move

		std::cout << copy_big.Cast<int>() << std::endl;
	}
}

template<typename T>
void Test1(T&& t)
{
	std::cout << typeid(T).name() << std::endl;
}

struct Range
{
	uint32_t min;
	uint32_t max;
};

struct Tooptip
{
	std::string tip;
};

//void Print(Any any)
//{
//	Type type = any.GetType();
//	for (auto field type.GetFields())
//	{
//		if (field.GetType().IsArray())
//		{
//			Print(field.GetValueRef())
//		}
//		else {
//			std::cout << field.name << field.value << std::endl;
//		}
//	}
//}

int main()
{
	
	

	Test t;
	TypeData& data = Registry::Get().AddTypeData<Test>("Test");
	data.AddField<Test, int>("a", &Test::a, Range{ 0,100 }, Tooptip{ "this is a" });
	data.AddMethod<Test>("Print", &Test::Print);
	data.AddMethod<Test>("TestReturn", &Test::TestReturn);
	data.AddMethod<Test>("TestReturnRef", &Test::TestReturnRef);
	
	int aaa = 10;
	
	constexpr bool vvvv = std::is_const_v<std::remove_reference_t<const int&>>;

	auto& f = data.GetField("a");
	
	std::cout << "range max" << f.GetProperty<Range>()->max << std::endl;
	std::cout << "range max" << f.GetProperty<Tooptip>()->tip << std::endl;

	Any instance = t;
	int param = 10;
	f.SetValue(instance,param);
	Any v = f.GetValue(instance);
	auto iv = v.Cast<int>();
	
	Test& t2 = instance.Cast<Test>();

	auto& method = data.GetMethod("Print");
	
	int& ref_aaa = aaa;
	method.Invoke(instance, ref_aaa, ref_aaa);
	std::cout << aaa;

	auto& m1 = data.GetMethod("TestReturn");
	auto ret1 =m1.Invoke(instance, 10,20);
	std::cout << ret1.Cast<int>() <<std::endl;

	auto& m2 = data.GetMethod("TestReturnRef");
	auto ret2 = m2.Invoke(instance, 1, 2, 3);

	std::cout << v.Cast<int>() << std::endl;
	std::cout << f.GetValue(instance).Cast<int>() << std::endl;
	
	return 0;
}