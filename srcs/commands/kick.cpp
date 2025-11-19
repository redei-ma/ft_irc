#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <iostream>

/* 
**	KICK #canale user1 								// Kick singolo
**	KICK #canale user1,user2,user3        			// Kick multipli stesso canale
**	KICK #canale user1 :Comportamento scorretto		// Con motivo
**	KICK #ch1,#ch2 user1		       			   	// Kick stesso utente da più canali
*/

void kicking()
{
	//to do
}

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

	if (targetUser == executer)
	{
		std::string message = ":" + executer->getNickName() + "!" + executer->getUserName() + "@irc.rfg.com" + " KICK " + channel->getName() + " " + targetUser->getNickName() + " :You can't kick yourself";;
		executer->sendMessage(message);
		return ;
	}

	if (!channel->isMember(targetUser))
		return (ReplyHandler::errorHandler(ERR_USERNOTINCHANNEL, *executer, commandArgs[1], "KICK"));
	
	std::string reason = " :";
	if (commandArgs.size() == 3)
		reason += commandArgs[2];
	else
		reason += executer->getNickName(); //non sono sicuro
	
	std::string message = ":" + executer->getNickName() + "!" + executer->getUserName() + "@irc.rfg.com" + " KICK " + channel->getName() + " " + targetUser->getNickName() + reason;
	channel->broadcastMessage(message, NULL);

	channel->removeUser(targetUser);

	std::cout << "KICK command executed: " << executer->getNickName() << " kicked " << targetUser->getNickName() << " from " << channel->getName() << std::endl;

	return ;
}
