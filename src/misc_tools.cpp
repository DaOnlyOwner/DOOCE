#include "misc_tools.h"
#include <sstream>

std::vector<std::string> misc_tools::split(const std::string& str, char delim)
{
	std::istringstream stream(str);
	std::vector<std::string> out;
	std::string split_into;
	while (std::getline(stream, split_into, delim))
	{
		out.push_back(split_into);
	}
	return out;
}
