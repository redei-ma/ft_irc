/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpirozzi <gpirozzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 10:56:33 by gpirozzi          #+#    #+#             */
/*   Updated: 2025/11/11 14:13:03 by gpirozzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include <iostream>

#define REGISTERED_CMD(enum, name) commandMap[enum] = &CommandHandler::name##Command

CommandHandler::CommandHandler(Server& server) : server(server)
{
	initCommand();
}

CommandHandler::CommandHandler(const CommandHandler& other) : server(other.server), commandMap(other.commandMap) {}

void CommandHandler::initCommand()
{
	REGISTERED_CMD(JOIN, join);
	REGISTERED_CMD(PASS, pass);
	REGISTERED_CMD(NICK, nick);
	REGISTERED_CMD(USER, user);
	REGISTERED_CMD(PRVMSG, msg);
	REGISTERED_CMD(KICK, kick);
	REGISTERED_CMD(INVITE, invite);
	REGISTERED_CMD(TOPIC, topic);
	REGISTERED_CMD(MODE, mode);
}

t_command	CommandHandler::reconizeCommand(std::string command)
{
	const char  *commands[]{
		"PASS",
		"NICK",
		"USER",
		"JOIN",
		"PRVMSG",
		"KICK",
		"INVITE",
		"TOPIC",
		"MODE",
		"PING"
	};
	for (int i = 0; i < 11; i++)
	{
		if (command == commands[i])
			return static_cast<t_command>(i);
	}
	return NOT_FOUND;
}

std::vector<std::string>	splitArgs(std::string args)
{
	
}

void	CommandHandler::execCommand(std::string commandAndArgs)
{
	std::vector<std::string>	splittedArgs = splitArgs(commandAndArgs);

	t_command	commandToExec = NOT_FOUND;

	if (commandAndArgs.size() == 2)
		commandToExec = reconizeCommand(splittedArgs[0]);
	else if (commandAndArgs.size() == 3)
		commandToExec = reconizeCommand(splittedArgs[1]);

		if (commandToExec == NOT_FOUND)
	{
		std::cout << "Command not found" << std::endl;
		return ;
	}

	(this->*commandMap[commandToExec])(splittedArgs);
}