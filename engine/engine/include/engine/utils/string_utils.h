#pragma once

namespace light
{
	class StringUtils
	{
	public:
		static std::string ToEditorName(const std::string& name)
		{
			std::string result = name;
			for (auto& c : result)
			{
				if (c == '_')
				{
					c = ' ';
				}
			}
			return result;
		};
	};
}
