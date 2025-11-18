#include "CommandHandler.hpp"
#include <algorithm>
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
	REGISTERED_CMD(PRIVMSG, privmsg);
	REGISTERED_CMD(KICK, kick);
	REGISTERED_CMD(INVITE, invite);
	REGISTERED_CMD(TOPIC, topic);
	REGISTERED_CMD(MODE, mode);
	REGISTERED_CMD(EXIT, exit);
	// REGISTERED_CMD(PING, ping);
}

t_command	CommandHandler::recognizeCommand(std::string command)
{
	const char  *commands[10] = {
		"PASS",
		"NICK",
		"USER",
		"JOIN",
		"PRIVMSG",
		"KICK",
		"INVITE",
		"TOPIC",
		"MODE",
		"EXIT",
		// "PING"
	};
	for (int i = 0; i < 10; i++)
	{
		if (command == commands[i])
			return static_cast<t_command>(i);
	}
	return NOT_FOUND;
}

static void	splitArgs(std::vector<std::string>& splittedArgs, std::string args)
{
	//Da capire se limit deve essere ' :'
	std::string::iterator limit = std::find(args.begin(), args.end(), ':');

	std::istringstream			stream(std::string(args.begin(), limit));
	std::string					buffer;

	while (std::getline(stream, buffer, ' '))
	{
		splittedArgs.push_back(buffer);
	}
	if (limit != args.end())
	{
		//da vedere se esplode
		limit++;
		splittedArgs.push_back(std::string(limit, args.end()));
	}
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

// void	CommandHandler::pingCommand(User* executer, std::vector<std::string> commandArgs)
// {
// 	(void)executer;
// 	(void)commandArgs;
// 	return ;
// }

void	CommandHandler::execCommand(User* executer, std::string input)
{
	std::vector<std::string>	splittedCommands;
	splitCommand(splittedCommands, input);

	for (size_t i = 0; i < splittedCommands.size(); i++)
	{
		std::vector<std::string> splittedArgs;
		splitArgs(splittedArgs, splittedCommands[i]);

		t_command	commandToExec = recognizeCommand(splittedArgs[0]);
		if (commandToExec == NOT_FOUND)
		{
			ReplyHandler::errorHandler(ERR_UNKNOWNCOMMAND, *executer, splittedArgs[1], splittedArgs[0]);
			continue ;
		}

		std::string	command = *splittedArgs.begin();
		splittedArgs.erase(splittedArgs.begin());

		(this->*commandMap[commandToExec])(executer, splittedArgs);
	}
}

/* ================DESTRUCTOR================ */
CommandHandler::~CommandHandler() {}
