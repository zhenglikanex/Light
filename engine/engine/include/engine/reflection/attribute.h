#pragma once

#include <string>
#include <cstdint>

namespace light::meta::attribute
{
	struct ShowInEditor
	{
		bool visible = true;
	};

	struct NameInEditor
	{
		std::string name;
	};

	struct EditorBindFunc
	{
		std::string func_name;
	};

	struct EditorCondition
	{
		std::string field_name;
		uint64_t value;

		EditorCondition(const std::string& field_name, uint64_t value) : field_name(field_name), value(value) {}
	};

	
}