#include "CommandHandler.hpp"
#include "utils.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

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
	REGISTERED_CMD(PASS, pass);
	REGISTERED_CMD(NICK, nick);
	REGISTERED_CMD(JOIN, join);
	REGISTERED_CMD(USER, user);
	REGISTERED_CMD(PRIVMSG, privmsg);
	REGISTERED_CMD(KICK, kick);
	REGISTERED_CMD(INVITE, invite);
	REGISTERED_CMD(TOPIC, topic);
	REGISTERED_CMD(MODE, mode);
	REGISTERED_CMD(PART, part);
	REGISTERED_CMD(QUIT, quit);
	REGISTERED_CMD(BOT, bot);
	REGISTERED_CMD(PING, ping);
}

t_command	CommandHandler::recognizeCommand(std::string command)
{
	const char  *commands[] = {
		"PASS",
		"NICK",
		"USER",
		"JOIN",
		"PRIVMSG",
		"KICK",
		"INVITE",
		"TOPIC",
		"MODE",
		"PART",
		"QUIT",
		"CAP",
		"WHO",
		"BOT",
		"PING",
	};
	for (int i = 0; i < 14; i++)
	{
		if (command == commands[i])
			return static_cast<t_command>(i);
	}
	return NOT_FOUND;
}

static std::vector<std::string>	splitArgs(std::string args)
{
	std::vector<std::string> splittedArgs;
	size_t	pos = args.find(" :");
	
	if (pos != std::string::npos)
	{
		splittedArgs = split(args.substr(0, pos), ' ');
		splittedArgs.push_back(args.substr(pos + 2));
	}
	else
		splittedArgs = split(args, ' ');

	return splittedArgs;
}

static void	splitCommand(std::vector<std::string>& splittedCommands, std::string input)
{
	std::string::size_type start = 0;
	std::string::size_type pos;

	while ((pos = input.find("\r\n", start)) != std::string::npos)
	{
		splittedCommands.push_back(input.substr(start, pos - start));
		start = pos + 2; // salta "\r\n"
	}
}

void	CommandHandler::execCommand(User* executer, std::string input)
{
	std::vector<std::string>	splittedCommands;

	if (input.empty())
		return ;

	splitCommand(splittedCommands, input);

	for (size_t i = 0; i < splittedCommands.size(); i++)
	{
		std::vector<std::string> splittedArgs = splitArgs(splittedCommands[i]);

		t_command	commandToExec = recognizeCommand(splittedArgs[0]);

		if (commandToExec == CAP || commandToExec == WHO)
			continue ;
		else if (commandToExec == NOT_FOUND)
		{
			ReplyHandler::errorHandler(ERR_UNKNOWNCOMMAND, *executer, "", splittedArgs[0]);
			continue ;
		}

		std::string	command = *splittedArgs.begin();
		splittedArgs.erase(splittedArgs.begin());

		(this->*commandMap[commandToExec])(executer, splittedArgs);
	}
}

/* ================DESTRUCTOR================ */
CommandHandler::~CommandHandler() {}
