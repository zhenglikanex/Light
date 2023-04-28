#pragma once

#include <string>

namespace light::meta
{
	class Enum
	{
	public:
		Enum() = default;
		Enum(std::string_view name, uint64_t value);

		std::string_view GetName() const { return name_; }

		template<typename T>
		T GetValue() const requires (std::is_enum_v<T> || std::is_arithmetic_v<T>)
		{
			return static_cast<T>(value_);
		}

		int64_t GetValue() const { return value_; }

		bool IsValid() const { return !name_.empty(); }
	private:
		std::string name_;
		int64_t value_ = 0;
	};
}