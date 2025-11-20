#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <iostream>

void	CommandHandler::inviteCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (!executer->getIsAuthenticated())
		return (ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "INVITE"));

	if (commandArgs.size() != 2)
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "INVITE"));

	User* invitedUser = _server.getUserByNick(commandArgs[0]);
	if (!invitedUser)
		return (ReplyHandler::errorHandler(ERR_NOSUCHNICK, *executer, commandArgs[0], "INVITE"));

	Channel *channel = _server.getChannelByName(commandArgs[1]);
	if (!channel)
		return (ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, commandArgs[1], "INVITE"));
	if (!channel->isMember(executer))
		return (ReplyHandler::errorHandler(ERR_NOTONCHANNEL, *executer, commandArgs[1], "INVITE"));
	if (channel->isMember(invitedUser))
		return (ReplyHandler::errorHandler(ERR_USERONCHANNEL, *executer, commandArgs[0], "INVITE"));

	if (channel->isInviteOnly() && !channel->isOperator(executer))
		return (ReplyHandler::errorHandler(ERR_CHANOPRIVSNEEDED, *executer, commandArgs[1], "INVITE"));

	channel->inviteUser(invitedUser);
	std::string message = executer->getPrefix() + " INVITE " + invitedUser->getNickName() + " :" + channel->getName();
	invitedUser->sendMessage(message);
	
	std::cout << "INVITE command executed: " << executer->getNickName() << " invited " << invitedUser->getNickName() << " to " << channel->getName() << std::endl;

	return (ReplyHandler::replyHandler(RPL_INVITEING, *executer, channel, invitedUser));
}