#include "CommandHandler.hpp"

t_status	CommandHandler::inviteCommand(User* executer, std::vector<std::string> commandArgs)
{
	if (!executer->getIsAuthenticated())
		return ERR_NOTREGISTERED;

	if (commandArgs.size() < 2)
		return ERR_NEEDMOREPARAMS;
	else if (commandArgs.size() > 2)
		return ERR_TOOMANYPARAMS;

	User* invitedUser = _server.getUser(commandArgs[0]);
	if (!invitedUser)
		return ERR_NOSUCHNICK;

	Channel *channel = _server.getChannel(commandArgs[1]);
	if (!channel)
		return ERR_NOSUCHCHANNEL;

	if (!channel->isMember(executer))
		return ERR_NOTONCHANNEL;

	if (channel->isMember(invitedUser))
		return ERR_USERONCHANNEL;

	if (channel->isInviteOnly() && !channel->isOperator(executer))
		return ERR_CHANOPRIVSNEEDED;

	channel->addInvitedUser(invitedUser);
	invitedUser->sendMessage(":" + executer->getNickName() + " INVITE " + invitedUser->getNickName() + " :" + channel->getName() + "\r\n");

	std::string message = ":irc.rfg.com 341 " + executer->getNickName() + " " + invitedUser->getNickName() + " " + channel->getName() + "\r\n";
	executer->sendMessage(message);
	// return RPL_INVITING;

	return SUCCESS;
}
