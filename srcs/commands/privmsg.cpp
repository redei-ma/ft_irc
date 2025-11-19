#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <algorithm>
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
			std::string msg = ":" + executer->getNickName() + " PRVMSG " + tmpChannel->getName() + ": " + commandArgs[1];
			tmpChannel->broadcastMessage(msg);
		}
		else
		{
			User *tmpUser = _server.getUserByNick(argString[i]);
			if (!tmpUser)
			{
				ReplyHandler::errorHandler(ERR_NOSUCHNICK, *executer, argString[i], "PRVMSG");
				continue ;
			}
			std::string msg = ":" + executer->getNickName() + " PRVMSG " + tmpUser->getNickName() + ": " + commandArgs[1];
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