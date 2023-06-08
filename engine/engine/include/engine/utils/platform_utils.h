#pragma once

#include "light_pch.h"

namespace light
{
	class FileDialogs
	{
	public:
		static std::string OpenFile(std::string_view filter);
		static std::string SaveFile(std::string_view default_name,std::string_view filter);
	};
}