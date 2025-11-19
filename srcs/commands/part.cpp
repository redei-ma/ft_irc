#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"

#include <sstream>

static std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, delimiter))
		tokens.push_back(token);
	
	if (tokens.empty())
		tokens.push_back(s);
    return tokens;
}

void CommandHandler::partCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (!executer->getIsAuthenticated())
	{
		ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "PART", "");
		return;
	}
	if (commandArgs.size() < 1 || commandArgs.size() > 2)
	{
		ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "PART", "");
		return;
	}
	std::string partMessage = (commandArgs.size() == 2) ? commandArgs[1] : "";
	std::vector<std::string> channelNames = split(commandArgs[0], ',');
	for (size_t i = 0; i < channelNames.size(); i++)
	{
		Channel* channel = _server.getChannelByName(channelNames[i]);
		if (!channel)
		{
			ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, channelNames[i], "PART");
			continue;
		}
		if (!channel->isMember(executer))
		{
			ReplyHandler::errorHandler(ERR_NOTONCHANNEL, *executer, channelNames[i], "PART");
			continue;
		}
		std::string msg = ":" + executer->getNickName() + " PART " + channel->getName();
		if (!partMessage.empty())
			msg += " :" + partMessage;
		channel->broadcastMessage(msg);
		channel->removeUser(executer);
		if (channel->getUserCount() == 0)
			_server.deleteChannel(channel);
	}
	return;
}

// void CommandHandler::partCommand(User* executer, std::vector<std::string>& commandArgs)
// {
// 	(void)commandArgs;
// 	(void)executer;
// }