#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "utils.hpp"
#include <algorithm>


void        sendMessages(Server &_server, User* executer, std::vector<std::string>& commandArgs)
{
	std::vector<std::string> argString = split(commandArgs[0], ',');
	for (size_t i = 0; i < argString.size(); i++)
	{
		if (argString[i][0] == '#')
		{   
			Channel     *tmpChannel = _server.getChannelByName(argString[i]);
			if (!tmpChannel)
			{
				ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, argString[i], "PRVMSG");
				continue ;
			}
			if (!tmpChannel->isMember(executer))
			{
				ReplyHandler::errorHandler(ERR_NOTONCHANNEL, *executer, argString[i], "PRVMSG");
				continue ;
			}
			std::string msg = ":" + executer->getNickName() + "!" + executer->getUserName() + "@" + "irc.rfg.com" + " PRIVMSG " + tmpChannel->getName() + " :" + commandArgs[1];
			tmpChannel->broadcastMessage(msg, executer); // lo devo mandare anche a me??
		}
		else
		{
			User *tmpUser = _server.getUserByNick(argString[i]);
			if (!tmpUser)
			{
				ReplyHandler::errorHandler(ERR_NOSUCHNICK, *executer, argString[i], "PRVMSG");
				continue ;
			}
			std::string msg = ":" + executer->getNickName() + "!" + executer->getUserName() + "@" + "irc.rfg.com" + " PRIVMSG " + tmpUser->getNickName() + " :" + commandArgs[1];
			tmpUser->sendMessage(msg);
		}
	}
	return ;
}

void	CommandHandler::privmsgCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (!executer->getIsAuthenticated())
		return (ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "PRVMSG"));

	if (commandArgs.size() != 2)
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "PRVMSG"));

	sendMessages(_server, executer, commandArgs);
	return ;
}