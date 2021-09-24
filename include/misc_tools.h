#pragma once
#include <vector>
#include <string>

namespace misc_tools
{
	// https://stackoverflow.com/questions/236129/how-do-i-iterate-over-the-words-of-a-string?page=1&tab=votes#tab-top
	std::vector<std::string> split(const std::string& str, char delim);
	/*{
		std::istringstream stream(str);
		std::vector<std::string> out;
		std::string split_into;
		while (std::getline(stream, split_into, delim))
		{
			out.push_back(split_into);
		}
		return out;
	}*/
}
