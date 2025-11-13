/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fzuccaro <fzuccaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 16:42:12 by gpirozzi          #+#    #+#             */
/*   Updated: 2025/11/12 17:48:51 by fzuccaro         ###   ########.fr       */
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
    if (errno == ERANGE || *endptr != '\0' || (portNbr < 6660 && portNbr > 6699))
    {
        std::cerr << "Invalid port number" << std::endl;
        return (2);
    }
    std::string input2(argv[2]);
    Server  myServer(portNbr, input2);
}