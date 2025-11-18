#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"

void	CommandHandler::kickCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (!executer->getIsAuthenticated())
		return (ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "KICK"));

	if (commandArgs.size() < 2 || commandArgs.size() > 3)
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "KICK"));
	
	Channel *channel = _server.getChannelByName(commandArgs[0]);
	if (!channel)
		return (ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, commandArgs[0], "KICK"));
	
	if (!channel->isMember(executer))
		return (ReplyHandler::errorHandler(ERR_NOTONCHANNEL, *executer, commandArgs[0], "KICK"));
	
	if (!channel->isOperator(executer))
		return (ReplyHandler::errorHandler(ERR_CHANOPRIVSNEEDED, *executer, commandArgs[0], "KICK"));
	
	User* targetUser = _server.getUserByNick(commandArgs[1]);
	if (!targetUser)
		return (ReplyHandler::errorHandler(ERR_NOSUCHNICK, *executer, commandArgs[1], "KICK"));
	
	if (!channel->isMember(targetUser))
		return (ReplyHandler::errorHandler(ERR_USERNOTINCHANNEL, *executer, commandArgs[1], "KICK"));

	std::string reason;
	if (commandArgs.size() == 3)
		reason = " :" + commandArgs[2];
	else
		reason = "";

	std::string message = ":" + executer->getNickName() + " KICK " + channel->getName() + " " + targetUser->getNickName() + reason;
	channel->broadcastMessage(message, NULL);

	channel->removeUser(targetUser);

	// if (channel->getUsers().empty())
	// 	_server.deleteChannel(channel);

	return ;
}