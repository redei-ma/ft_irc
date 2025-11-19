#include "utils.hpp"
#include <sstream>

std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, delimiter))
		tokens.push_back(token);
	
	if (tokens.empty())
		tokens.push_back(s);
    return tokens;
}
