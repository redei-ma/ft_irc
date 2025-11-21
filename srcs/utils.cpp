#include "utils.hpp"
#include <sstream>

std::vector<std::string> split(const std::string &s, char delimiter)
{
		std::vector<std::string> tokens;
		std::stringstream ss(s);
		std::string buffer;

		while (std::getline(ss, buffer, delimiter))
			tokens.push_back(buffer);

		return tokens;
}
