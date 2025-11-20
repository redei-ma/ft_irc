#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "ReplyHandler.hpp"
#include <iostream>

const std::string _BOTprefix = ":irc.rfg.com BOT:\n";

#define REGISTERED_BOT_CMD(enum, name) botMapExecuter[enum] = name##Command

typedef enum s_bot
{
	HELP,
	USERS,
	CHANNELS,
	ERROR,
}	t_bot;

t_bot	findCommandToExec(const std::string& input)
{
	const char* botCommands[3] = {
		"/help",
		"/users",
		"/channels"
	};

	for (size_t i = 0; i < 3; i++)
	{
		if (input == botCommands[i])
			return (static_cast<t_bot>(i));
	}
	return (ERROR);
}

static void	sendBotCommands(User* executer)
{
	std::string msg = ":" + executer->getNickName() + "!" + executer->getUserName() + "@" + executer->getHostNameAsString() +
	" Use the command BOT with arg:\n" +
	"/help\n" + 
	"/users\n" +
	"/channels";
	executer->sendMessage(msg);
}

void	helpCommand(User* executer, const Server& server)
{
	(void)server;

	std::string msg = _BOTprefix +
	"NICK with something for change your nickname\n" +
	"INVITE with nickName and #nameChannel for invite a user to a channel\n" +
	"JOIN with #nameChannel for join/create a channel\n" +
	"KICK with nickName and #nameChannel for kick a user from a channel\n" +
	"MODE with #nameChannel and modes(+/- i t k o l) for set modes to a channel\n" +
	"TOPIC with #nameChannel and topic for set a topic to a channel\n"
	"PRIVMSG with nickName/#nameChannel and message for send a message to a user/channel\n" +
	"PART with #nameChannel for leave a channel\n" +
	"QUIT for disconnect from server\n" +
	"BOT with arg: /help | /users | /channels\n";
	executer->sendMessage(msg);
}

void	usersCommand(User* executer, const Server& _server)
{
	std::vector<User*>	userVector = _server.getUserVector();
	std::string	msg = _BOTprefix;

	for (size_t i = 0; i < userVector.size(); i++)
	{
		msg += userVector[i]->getNickName();
		if (i < userVector.size() - 1)
			msg += "\n";
	}
	executer->sendMessage(msg);
}

void	channelsCommand(User* executer, const Server& _server)
{
	std::vector<Channel*>	channelVector = _server.getChannelVector();
	std::string	msg = _BOTprefix;

	if (channelVector.empty())
	{
		msg += "No channels created";
		executer->sendMessage(msg);
		return ;
	}

	for (size_t i = 0; i < channelVector.size(); i++)
	{
		msg += channelVector[i]->getName();
		if (i < channelVector.size() - 1)
			msg += "\n";
	}
	executer->sendMessage(msg);
}

static void	setBotMapExecuter(std::map<t_bot, void(*)(User*, const Server&)>& botMapExecuter)
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
		std::string msg = _BOTprefix + executer->getNickName() + "!" +
		executer->getUserName() + "@" + executer->getHostNameAsString() + " BOT " + " :" + "Invalid command";
		executer->sendMessage(msg);
		return ;
	}

	std::map<t_bot, void(*)(User*, const Server&)> botMapExecuter;
	setBotMapExecuter(botMapExecuter);

	botMapExecuter[command](executer, _server);
}
