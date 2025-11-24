#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <cstdlib>
#include <cerrno>
#include <algorithm>
#include <iostream>

static bool isValidFlag(char c)
{
	return (c == 'k' || c == 'i' || c == 'o' || c == 't' || c == 'l');
}

static bool flagNeedsParam(char flag, bool adding)
{
	if (adding)
		return (flag == 'k' || flag == 'l' || flag == 'o');
	else
		return (flag == 'o');
}

static void applyInviteMode(Channel* channel, User* executer, bool add)
{
	channel->setInviteOnly(add);
	std::string mode = add ? "+i" : "-i";
	std::string message = ":" + executer->getPrefix() + " MODE " + channel->getName() + " " + mode;
	channel->broadcastMessage(message);
}

static void applyTopicMode(Channel* channel, User* executer, bool add)
{
	channel->setTopicRestriction(add);
	std::string mode = add ? "+t" : "-t";
	std::string message = ":" + executer->getPrefix() + " MODE " + channel->getName() + " " + mode;
	channel->broadcastMessage(message);
}

static void applyKeyMode(Server& _server, Channel* channel, User* executer, bool add, std::string& param)
{
	if (add)
	{
		if (!channel->getPassword().empty())
			return (ReplyHandler::errorHandler(ERR_KEYSET, *executer, channel->getName(), "MODE"));
		channel->setPassword(param);
		std::string message = ":" + executer->getPrefix() + " MODE " + channel->getName() + " +k " + param;
		channel->broadcastMessage(message);
	}
	else
	{
		channel->removePassword();
		std::string message = ":" + executer->getPrefix() + " MODE " + channel->getName() + " -k";
		channel->broadcastMessage(message);
	}
}

static void applyOperatorMode(Server& _server, Channel* channel, User* executer, bool add, std::string& param)
{
	User* target = _server.getUserByNick(param);
	if (!target)
		return (ReplyHandler::errorHandler(ERR_NOSUCHNICK, *executer, param, "MODE"));

	if (!channel->isMember(target))
		return (ReplyHandler::errorHandler(ERR_USERNOTINCHANNEL, *executer, param, "MODE"));

	if (add)
	{
		channel->addOperator(target);
		std::string message = ":" + executer->getPrefix() + " MODE " + channel->getName() + " +o " + param;
		channel->broadcastMessage(message);
	}
	else
	{
		channel->removeOperator(target);
		std::string message = ":" + executer->getPrefix() + " MODE " + channel->getName() + " -o " + param;
		channel->broadcastMessage(message);
	}
}

static void applyLimitMode(Server& _server, Channel* channel, User* executer, 
						  bool add, std::string& param)
{
	if (add)
	{
		char* endptr;
		errno = 0;
		long result = std::strtol(param.c_str(), &endptr, 10);
		
		if (*endptr != '\0' || errno == ERANGE || result > MAX_USER || result <= 0)
			return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));

		channel->setUsersLimit(result);
		std::string message = ":" + executer->getPrefix() + " MODE " + channel->getName() + " +l " + param;
		channel->broadcastMessage(message);
	}
	else
	{
		channel->removeUsersLimit();
		std::string message = ":" + executer->getPrefix() + " MODE " + channel->getName() + " -l";
		channel->broadcastMessage(message);
	}
}

void CommandHandler::modeCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (!executer->getIsAuthenticated())
		return ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "MODE");

	if (commandArgs.empty())
		return ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE");

	Channel* channel = _server.getChannelByName(commandArgs[0]);
	if (!channel)
		return ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, commandArgs[0], "MODE");

	if (!channel->isMember(executer))
		return ReplyHandler::errorHandler(ERR_NOTONCHANNEL, *executer, commandArgs[0], "MODE");

	if (!channel->isOperator(executer))
		return ReplyHandler::errorHandler(ERR_CHANOPRIVSNEEDED, *executer, commandArgs[0], "MODE");

	if (commandArgs.size() == 1)
		return (ReplyHandler::replyHandler(RPL_CHANNELMODEIS, *executer, channel, NULL));

	std::string modeString = commandArgs[1];
	bool adding = true;
	size_t paramIndex = 2;

	for (size_t i = 0; i < modeString.length(); i++)
	{
		char c = modeString[i];
		if (c == '+')
		{
			adding = true;
			continue;
		}
		else if (c == '-')
		{
			adding = false;
			continue;
		}

		if (!isValidFlag(c))
		{
			ReplyHandler::errorHandler(ERR_UNKNOWNMODE, *executer, std::string(1, c), "MODE");
			continue;
		}

		std::string param = "";
		if (flagNeedsParam(c, adding))
		{
			if (paramIndex < commandArgs.size() && !commandArgs[paramIndex].empty())
			{
				param = commandArgs[paramIndex];
				paramIndex++;
			}
			else
			{
				ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE");
				continue;
			}
		}

		switch (c)
		{
			case 'i':
				applyInviteMode(channel, executer, adding);
				break;
			case 't':
				applyTopicMode(channel, executer, adding);
				break;
			case 'k':
				applyKeyMode(_server, channel, executer, adding, param);
				break;
			case 'o':
				applyOperatorMode(_server, channel, executer, adding, param);
				break;
			case 'l':
				applyLimitMode(_server, channel, executer, adding, param);
				break;
		}
	}
}