/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpirozzi <gpirozzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 16:42:12 by gpirozzi          #+#    #+#             */
/*   Updated: 2025/11/14 15:36:47 by gpirozzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdlib>
#include <errno.h>
#include <iostream>

int main(int argc, char **argv)
{
    signal(SIGINT, handle_sigint);
    
    if (argc != 3)
    {
        std::cerr << "Invalid input" << std::endl;
        return (1);
    }
    char    *endptr;
    int portNbr = std::strtol(argv[1], &endptr,10);
    if (errno == ERANGE || *endptr != '\0' || (portNbr < 6660 || portNbr > 6699))
    {
        std::cerr << "Invalid port number" << std::endl;
        return (2);
    }
    std::string input2(argv[2]);
	try
	{
        for (size_t i = 0; i < input2.size(); i++)
        {
            if (std::isspace(input2[2]) || std::isprint(input2[2]))
                throw std::invalid_argument("Invalid characher in password");
        }
		Server  myServer(portNbr, input2);
		myServer.run();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}