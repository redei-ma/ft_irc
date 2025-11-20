#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <iostream>

void	CommandHandler::topicCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (!executer->getIsAuthenticated())
		return (ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "TOPIC"));

	if (commandArgs.size() < 1 || commandArgs.size() > 2)
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "TOPIC"));
	else
	{
		Channel *channel = _server.getChannelByName(commandArgs[0]);
		if (!channel)
			return (ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, commandArgs[0], "TOPIC"));

		if (!channel->isMember(executer))
			return (ReplyHandler::errorHandler(ERR_NOTONCHANNEL, *executer, commandArgs[0], "TOPIC"));

		if (commandArgs.size() == 1)
		{
			if (!channel->hasTopic())
				return (ReplyHandler::replyHandler(RPL_NOTOPIC, *executer, channel, NULL));
			else 
				return (ReplyHandler::replyHandler(RPL_TOPIC, *executer, channel, NULL));
		}
		else if (commandArgs.size() == 2)
		{
			if (channel->isTopicRestricted() && !channel->isOperator(executer))
				return (ReplyHandler::errorHandler(ERR_CHANOPRIVSNEEDED, *executer, commandArgs[0], "TOPIC"));
			else
			{
				channel->setTopic(commandArgs[1]);
				std::string message = executer->getPrefix() + " TOPIC " + channel->getName() + " :" + commandArgs[1];
				channel->broadcastMessage(message);
			}
		}
	}
	std::cout << "TOPIC command executed: " << executer->getNickName() << " set topic for " << commandArgs[0] << std::endl;
	return ;
}