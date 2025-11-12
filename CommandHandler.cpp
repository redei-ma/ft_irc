/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpirozzi <gpirozzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 10:56:33 by gpirozzi          #+#    #+#             */
/*   Updated: 2025/11/12 12:33:04 by gpirozzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

/* ================MACRO================ */
#define REGISTERED_CMD(enum, name) commandMap[enum] = &CommandHandler::name##Command

/* ================CONSTRUCTOR================ */
CommandHandler::CommandHandler(Server& server) : _server(server)
{
	initCommand();
}

CommandHandler::CommandHandler(const CommandHandler& other) : _server(other._server), commandMap(other.commandMap) {}

/* ================METHODS================ */
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
	std::cout << "morte" << std::endl;
	const char  *commands[10] = {
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
	std::cout << "morte2" << std::endl;
	for (int i = 0; i < 10; i++)
	{
		std::cout << "morte3" << std::endl;
		if (command == commands[i])
			return static_cast<t_command>(i);
	}
	std::cout << "morte4" << std::endl;
	return NOT_FOUND;
}

void	splitArgs(std::vector<std::string>& splittedArgs, std::string args)
{
	std::string::iterator limit = std::find(args.begin(), args.end(), ':');

	std::istringstream			stream(std::string(args.begin(), limit));
	std::string					buffer;

	while (std::getline(stream, buffer, ' '))
	{
		splittedArgs.push_back(buffer);
	}
	if (limit != args.end())
	{
		splittedArgs.push_back(std::string(limit, args.end()));
	}
}

void	splitCommand(std::vector<std::string>& splittedCommands, std::string input)
{
	std::string::iterator	commandLimit = input.begin();
	std::string::iterator	commandStart = input.begin();

	while (commandLimit != input.end())
	{
		commandLimit = std::find(commandStart, input.end(), '\n');
		splittedCommands.push_back(std::string(commandStart, commandLimit));
		++commandLimit;
		commandStart = commandLimit;
	}
}

void	CommandHandler::passCommand(std::vector<std::string> commandArgs)
{
	for (size_t i = 0; i < commandArgs.size(); i++)
	{
		std::cout << "ARG["<<i<<"]" << commandArgs[i] << std::endl;
	}
}

void	CommandHandler::nickCommand(std::vector<std::string> commandArgs)
{

}

void	CommandHandler::userCommand(std::vector<std::string> commandArgs)
{

}

void	CommandHandler::joinCommand(std::vector<std::string> commandArgs)
{

}

void	CommandHandler::msgCommand(std::vector<std::string> commandArgs)
{

}

void	CommandHandler::kickCommand(std::vector<std::string> commandArgs)
{

}

void	CommandHandler::inviteCommand(std::vector<std::string> commandArgs)
{

}

void	CommandHandler::topicCommand(std::vector<std::string> commandArgs)
{

}

void	CommandHandler::modeCommand(std::vector<std::string> commandArgs)
{

}

void	CommandHandler::pingCommand(std::vector<std::string> commandArgs)
{

}

void	CommandHandler::execCommand(User* executer, std::string input)
{
	std::vector<std::string>	splittedCommands;
	splitCommand(splittedCommands, input);

	for (size_t i = 0; i < splittedCommands.size(); i++)
	{
		std::vector<std::string> splittedArgs;
		splitArgs(splittedArgs, splittedCommands[i]);

		t_command	commandToExec = NOT_FOUND;
		commandToExec = reconizeCommand(splittedArgs[0]);
		if (commandToExec == NOT_FOUND)
		{
			std::cout << "Command not found" << std::endl;
			return ;
		}
		splittedArgs.erase(splittedArgs.begin());
		(this->*commandMap[commandToExec])(splittedArgs);
	}
}

/* ================DESTRUCTOR================ */
CommandHandler::~CommandHandler() {}