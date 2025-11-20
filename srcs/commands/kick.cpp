#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "utils.hpp"
#include <iostream>

/* 
**	KICK #canale user1 :motivo					// Kick singolo
**	KICK #canale user1,user2,user3 :motivo		// Kick multipli stesso canale
**	KICK #ch1,#ch2 user1 :motivo				// Kick stesso utente da più canali
**	KICK #ch1,#ch2 user1,user2 :motivo			// Utente e canali abbinati
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

	std::vector<std::string> channelNames = split(commandArgs[0], ',');
	std::vector<std::string> usersName = split(commandArgs[1], ',');

	if (channelNames.size() == usersName.size())
	{
		for (size_t i = 0; i > channelNames.size(); i++)
		{
			Channel *channel = _server.getChannelByName(channelNames[i]);
			if (!channel)
				return (ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, channelNames[i], "KICK"));
			
			if (!channel->isMember(executer))
				return (ReplyHandler::errorHandler(ERR_NOTONCHANNEL, *executer, channelNames[i], "KICK"));
			
			if (!channel->isOperator(executer))
				return (ReplyHandler::errorHandler(ERR_CHANOPRIVSNEEDED, *executer, channelNames[i], "KICK"));

			User* targetUser = _server.getUserByNick(usersName[i]);
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
			
			kicking(executer, channel, targetUser, reason);
		}
	}
	else if (channelNames.size() == 1)
	{

	}
	else if (usersName.size() == 1)
	{

	}
	else
	{
		return ERROR;
	}
}



// void	CommandHandler::kickCommand(User* executer, std::vector<std::string>& commandArgs)
// {
// 	if (!executer->getIsAuthenticated())
// 		return (ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "KICK"));

// 	if (commandArgs.size() < 2 || commandArgs.size() > 3)
// 		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "KICK"));
	
// 	Channel *channel = _server.getChannelByName(commandArgs[0]);
// 	if (!channel)
// 		return (ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, commandArgs[0], "KICK"));
	
// 	if (!channel->isMember(executer))
// 		return (ReplyHandler::errorHandler(ERR_NOTONCHANNEL, *executer, commandArgs[0], "KICK"));
	
// 	if (!channel->isOperator(executer))
// 		return (ReplyHandler::errorHandler(ERR_CHANOPRIVSNEEDED, *executer, commandArgs[0], "KICK"));

// 	User* targetUser = _server.getUserByNick(commandArgs[1]);
// 	if (!targetUser)
// 		return (ReplyHandler::errorHandler(ERR_NOSUCHNICK, *executer, commandArgs[1], "KICK"));

// 	if (targetUser == executer)
// 	{
// 		std::string message = ":" + executer->getNickName() + "!" + executer->getUserName() + "@irc.rfg.com" + " KICK " + channel->getName() + " " + targetUser->getNickName() + " :You can't kick yourself";;
// 		executer->sendMessage(message);
// 		return ;
// 	}

// 	if (!channel->isMember(targetUser))
// 		return (ReplyHandler::errorHandler(ERR_USERNOTINCHANNEL, *executer, commandArgs[1], "KICK"));
	
// 	std::string reason = " :";
// 	if (commandArgs.size() == 3)
// 		reason += commandArgs[2];
// 	else
// 		reason += executer->getNickName();
	
// 	std::string message = ":" + executer->getNickName() + "!" + executer->getUserName() + "@irc.rfg.com" + " KICK " + channel->getName() + " " + targetUser->getNickName() + reason;
// 	channel->broadcastMessage(message, NULL);

// 	channel->removeUser(targetUser);

// 	std::cout << "KICK command executed: " << executer->getNickName() << " kicked " << targetUser->getNickName() << " from " << channel->getName() << std::endl;

// 	return ;
// }
