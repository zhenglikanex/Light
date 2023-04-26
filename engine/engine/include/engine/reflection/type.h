#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace light::meta
{
	class Field;
	class Method;
	class TypeData;

	class Type
	{
	public:
		static Type Get(std::string_view name);

		Type() = default;

		bool IsVaild() const { return data_ != nullptr; }

		std::string_view GetName() const;

		const Field& GetField(std::string_view name) const;
		const Method& GeMethod(std::string_view name) const;		
	private:
		friend class Registry;

		Type(const TypeData* data);
		const TypeData* data_ = nullptr;
	};
}