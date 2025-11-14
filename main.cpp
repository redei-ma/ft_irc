/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: redei-ma <redei-ma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 16:42:12 by gpirozzi          #+#    #+#             */
/*   Updated: 2025/11/14 15:45:25 by redei-ma         ###   ########.fr       */
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
        return (1);
    }
    std::string pass(argv[2]);
	// if (pass.empty())
	// {
	// 	std::cerr << "Invalid password" << std::endl;
    //     return (1);
	// }
	
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