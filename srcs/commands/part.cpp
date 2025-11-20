#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "utils.hpp"

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
	std::string reason = (commandArgs.size() == 2) ? commandArgs[1] : "";
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
		std::string msg = executer->getPrefix() + " PART " + channel->getName();
		if (!reason.empty())
			msg += " :" + reason;
		channel->broadcastMessage(msg);
		channel->removeUser(executer);
		if (channel->getUserCount() == 0)
			_server.deleteChannel(channel);
	}
	return;
}
