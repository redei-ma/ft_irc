#include "CommandHandler.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

void	CommandHandler::prvmsgCommand(User* executer, std::vector<std::string>& commandArgs)
{
    if (!executer->getIsAuthenticated())
        return (ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "PRIVMSG"));

    if (commandArgs.size() != 2)
        return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "PRIVMSG"));

    if (std::find(commandArgs[0].begin(), commandArgs[0].end(), ',') != commandArgs[0].end())
    {   
        std::vector<std::string> argString = split(commandArgs[0], ',');
        for (size_t i = 0; i < argString.size(); i++)
        {
            if (argString[i][0] == '#')
            {   
                Channel     *tmpChannel = _server.getChannelByName(argString[i]);
                if (!tmpChannel)
                    return (ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, argString[i], "PRIVMSG"));
                if (!tmpChannel->isMember(executer))
                    return (ReplyHandler::errorHandler(ERR_NOTONCHANNEL, *executer, argString[i], "PRIVMSG"));
                std::string _msg = ":" + executer->getNickName() + " PRVMSG " + tmpChannel->getName() + ": " + commandArgs[1];
                tmpChannel->broadcastMessage(_msg, executer);
            }
            else
            {
                User *tmpUser = _server.getUserByNick(argString[i]);
                if (!tmpUser)
                    return (ReplyHandler::errorHandler(ERR_NOSUCHNICK, *executer, argString[i], "PRIVMSG"));
                std::string _msg = ":" + executer->getNickName() + " PRVMSG " + tmpUser->getNickName() + ": " + commandArgs[1];
                tmpUser->sendMessage(_msg);
            }
        }
    }
    else
    {
        if (commandArgs[0][0] == '#')
        {
            Channel     *tmpChannel = _server.getChannelByName(commandArgs[0]);
            if (!tmpChannel)
                return (ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, commandArgs[0], "PRIVMSG"));
            if (!tmpChannel->isMember(executer))
                return (ReplyHandler::errorHandler(ERR_NOTONCHANNEL, *executer, commandArgs[0], "PRIVMSG"));
            std::string _msg = ":" + executer->getNickName() + " PRVMSG " + tmpChannel->getName() + ": " + commandArgs[1];
                tmpChannel->broadcastMessage(_msg, executer);
        }
        else
        {
            User *tmpUser = _server.getUserByNick(commandArgs[0]);
            if (!tmpUser)
                return (ReplyHandler::errorHandler(ERR_NOSUCHNICK, *executer, commandArgs[0], "PRIVMSG"));
            std::string _msg = ":" + executer->getNickName() + " PRVMSG " + tmpUser->getNickName() + ": " + commandArgs[1];
            tmpUser->sendMessage(_msg);
        }
    }
	return ;
}