#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <iostream>

#define REGISTERED_BOT_CMD(enum, name) botMapExecuter[enum] = name##Command

typedef enum s_bot
{
	HELP,
	USERS,
	CHANNELS,
	GAME,
	ERROR,
}	t_bot;

t_bot	findCommandToExec(const std::string& input)
{
	const char* botCommands[4] {
		"/help",
		"/users",
		"/channels",
		"/game"
	};

	for (size_t i = 0; i < 4; i++)
	{
		if (input == botCommands[i])
			return (static_cast<t_bot>(i));
	}
	return (ERROR);
}

static void	sendBotCommands(User* executer)
{
	std::string msg = ":" + executer->getNickName() + "!" +
	executer->getUserName() + "@" + executer->getHostNameAsString() + " BOT " + " :"
	+ "Use the command BOT whit arg:\n" +
	"/help\n" + 
	"/users\n" +
	"/channels\n"
	"/game";
	executer->sendMessage(msg);
}

void	helpCommand(User* executer)
{
	std::string msg = ":" + executer->getNickName() + "!" +
	executer->getUserName() + "@" + executer->getHostNameAsString() + " BOT " + " :"
	+ "Use the command BOT whit arg:\n" +
	"/help\n" + 
	"/users\n" +
	"/channels\n"
	"/game";
}

void	usersCommand(User* executer)
{

}

void	channelsCommand(User* executer)
{

}

static void	setBotMapExecuter(std::map<t_bot, void(*)(User*)>& botMapExecuter)
{
	REGISTERED_BOT_CMD(HELP, help);
	REGISTERED_BOT_CMD(USERS, users);
	REGISTERED_BOT_CMD(CHANNELS, channels);
}

void	CommandHandler::botCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (!executer->getIsAuthenticated())
		return (ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "BOT"));

	if (commandArgs.size() > 1)
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "BOT"));

	if (commandArgs.size() == 0)
	{
		sendBotCommands(executer);
		return ;
	}

	t_bot	command = findCommandToExec(commandArgs[1]);
	if (command == ERROR)
	{
		std::string msg = ":" + executer->getNickName() + "!" +
		executer->getUserName() + "@" + executer->getHostNameAsString() + " BOT " + " :" + "Invalid command";
		executer->sendMessage(msg);
		return ;
	}

	std::map<t_bot, void(*)(User*)> botMapExecuter;
	setBotMapExecuter(botMapExecuter);

	botMapExecuter[command](executer);
}
