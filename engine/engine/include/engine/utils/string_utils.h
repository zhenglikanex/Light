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

		static std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters)
		{
			size_t start = 0;
			size_t end = string.find_first_of(delimiters);

			std::vector<std::string> result;

			while (end <= std::string::npos)
			{
				std::string token = string.substr(start, end - start);
				if (!token.empty())
					result.push_back(token);

				if (end == std::string::npos)
					break;

				start = end + 1;
				end = string.find_first_of(delimiters, start);
			}

			return result;
		}

	};
}
