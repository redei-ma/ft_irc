#include "Server.hpp"
#include <cstdlib>
#include <errno.h>
#include <iostream>

int main(int argc, char **argv)
{
	signal(SIGINT, handle_sigint);

	if (argc != 3)
	{
		std::cerr << "Usage: ./IRC <port> <password>" << std::endl;
		return (1);
	}

	char	*endptr;
	int		portNbr = std::strtol(argv[1], &endptr,10);
	if (errno == ERANGE || *endptr != '\0' || (portNbr < 6660 || portNbr > 6699))
	{
		std::cerr << "Invalid port number" << std::endl;
		return (1);
	}

	std::string pass(argv[2]);
	if (pass.empty())
	{
		std::cerr << "Empty password is not allowed" << std::endl;
		return (1);
	}
	for (size_t i = 0; i < pass.size(); i++)
	{
		if (std::isspace(pass[i]) || !std::isprint(pass[i]))
		{
			std::cerr << "Invalid characher in password" << std::endl;
			return (1);
		}
	}

	try
	{
		Server  myServer(portNbr, pass);
		myServer.run();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}