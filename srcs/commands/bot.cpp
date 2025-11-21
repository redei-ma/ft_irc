#include "config.hpp"
#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "ReplyHandler.hpp"
#include <iostream>

#define REGISTERED_BOT_CMD(enum, name) botMapExecuter[enum] = name##Command

static void sendBotLine(User* executer, const std::string& text)
{
	std::string msg = ":" + std::string(SERVER_NAME) + " NOTICE " + executer->getNickName() + " :" + text;
	executer->sendMessage(msg);
}

typedef enum s_bot
{
	HELP,
	USERS,
	CHANNELS,
	ERROR,
}	t_bot;

static t_bot	findCommandToExec(const std::string& input)
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

static void		sendBotCommands(User* executer)
{
	sendBotLine(executer, "Use the command BOT with arg:");
	sendBotLine(executer, "/help");
	sendBotLine(executer, "/users");
	sendBotLine(executer, "/channels");
}

static void	helpCommand(User* executer, const Server& server)
{
	(void)server;

	sendBotLine(executer, "NICK with something for change your nickname");
	sendBotLine(executer, "INVITE with nickName and #nameChannel for invite a user to a channel");
	sendBotLine(executer, "JOIN with #nameChannel for join/create a channel");
	sendBotLine(executer, "KICK with nickName and #nameChannel for kick a user from a channel");
	sendBotLine(executer, "MODE with #nameChannel and modes(+/- i t k o l) for set modes to a channel");
	sendBotLine(executer, "TOPIC with #nameChannel and topic for set a topic to a channel");
	sendBotLine(executer, "PRIVMSG with nickName/#nameChannel and message for send a message to a user/channel");
	sendBotLine(executer, "PART with #nameChannel for leave a channel");
	sendBotLine(executer, "QUIT for disconnect from server");
	sendBotLine(executer, "BOT with arg: /help | /users | /channels");
}

static void	usersCommand(User* executer, const Server& _server)
{
	std::vector<User*>	userVector = _server.getUserVector();
	if (userVector.empty())
	{
		sendBotLine(executer, "No users connected");
		return;
	}
	for (size_t i = 0; i < userVector.size(); i++)
	{
		if (executer == userVector[i])
			continue ;
		sendBotLine(executer, userVector[i]->getNickName());
	}
}

static void	channelsCommand(User* executer, const Server& _server)
{
	std::vector<Channel*>	channelVector = _server.getChannelVector();
	if (channelVector.empty())
	{
		sendBotLine(executer, "No channels currently");
		return;
	}
	for (size_t i = 0; i < channelVector.size(); i++)
	{
		sendBotLine(executer, channelVector[i]->getName());
	}
}

static void	setBotMapExecuter(std::map<t_bot, void(*)(User*, const Server&)>& botMapExecuter)
{
	static bool	isInitialized = false;
	if (isInitialized)
		return ;
	REGISTERED_BOT_CMD(HELP, help);
	REGISTERED_BOT_CMD(USERS, users);
	REGISTERED_BOT_CMD(CHANNELS, channels);
	isInitialized = true;
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

	t_bot	botCommand = findCommandToExec(commandArgs[1]);
	if (botCommand == ERROR)
	{
		sendBotLine(executer, "Invalid command");
		return ;
	}

	static std::map<t_bot, void(*)(User*, const Server&)> botMapExecuter;
	setBotMapExecuter(botMapExecuter);

	botMapExecuter[botCommand](executer, _server);
}
