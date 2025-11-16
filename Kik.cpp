#include "CommandHandler.hpp"

t_status	CommandHandler::kickCommand(User* executer, std::vector<std::string> commandArgs)
{
	if (!executer->getIsAuthenticated())
		return ERR_NOTREGISTERED;
	
	if (commandArgs.size() < 2)
		return ERR_NEEDMOREPARAMS;
	else if (commandArgs.size() > 3)
		return ERR_TOOMANYPARAMS;
	
	Channel *channel = _server.getChannel(commandArgs[0]);
	if (!channel)
		return ERR_NOSUCHCHANNEL;
	
	if (!channel->isMember(executer))
		return ERR_NOTONCHANNEL;
	
	if (!channel->isOperator(executer))
		return ERR_CHANOPRIVSNEEDED;
	
	User* targetUser = _server.getUser(commandArgs[1]);
	if (!targetUser)
		return ERR_NOSUCHNICK;
	
	if (!channel->isMember(targetUser))
		return ERR_USERNOTINCHANNEL;

	std::string reason;
	if (commandArgs.size() == 3)
		reason = " :" + commandArgs[2];
	else
		reason = "";
	std::string message = ":" + executer->getNickName() + " KICK " + channel->getName() + " " + targetUser->getNickName() + reason + "\r\n";
	channel->broadcastMessage(message, NULL);

	channel->removeUser(targetUser);
	// return RPL_INVITING;
	
	return SUCCESS;
}
