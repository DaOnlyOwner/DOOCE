#pragma once
#include <vector>
#include <string>

namespace misc_tools
{
	// https://stackoverflow.com/questions/236129/how-do-i-iterate-over-the-words-of-a-string?page=1&tab=votes#tab-top
	std::vector<std::string> split(const std::string& str, char delim);
}
