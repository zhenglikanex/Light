#pragma once

#include <string_view>
#include <string>
#include <chrono>

#include "spdlog/fmt/fmt.h"

namespace light
{
	template<class Fn>
	class Timer
	{
	public:
		Timer(std::string_view name, Fn&& func)
			: name_(name)
			, func_(func)
			, start_(std::chrono::high_resolution_clock::now())
		{
			
		}

		~Timer()
		{
			auto now = std::chrono::high_resolution_clock::now();
			auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - start_).count();
			
			func_({ name_, microseconds / 1000.0f });
		}
	private:
		std::string name_;
		Fn func_;
		std::chrono::high_resolution_clock::time_point start_;
	};
}