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

t_status	CommandHandler::msgCommand(User* executer, std::vector<std::string> commandArgs)
{
    if (!executer->getIsAuthenticated())
        return ERR_NOTREGISTERED;

    if (commandArgs.size() != 2)
        return ERR_NEEDMOREPARAMS;

    if (std::find(commandArgs[0].begin(), commandArgs[0].end(), ',') != commandArgs[0].end())
    {   
        std::vector<std::string> argString = split(commandArgs[0], ',');
        for (size_t i = 0; i < argString.size(); i++)
        {
            if (argString[i][0] == '#')
            {   
                Channel     *tmpChannel = _server.getChannelByName(argString[i]);
                if (!tmpChannel)
                    return ERR_NOSUCHCHANNEL;
                if (!tmpChannel->isMember(executer))
                    return ERR_NOTONCHANNEL;
                std::string _msg = ":" + executer->getNickName() + " PRVMSG " + tmpChannel->getName() + ": " + commandArgs[1];
                tmpChannel->broadcastMessage(_msg, executer);
            }
            else
            {
                User *tmpUser = _server.getUserByNick(argString[i]);
                if (!tmpUser)
                    return ERR_NOSUCHNICK;
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
                return ERR_NOSUCHCHANNEL;
            if (!tmpChannel->isMember(executer))
                return ERR_NOTONCHANNEL;
            std::string _msg = ":" + executer->getNickName() + " PRVMSG " + tmpChannel->getName() + ": " + commandArgs[1];
                tmpChannel->broadcastMessage(_msg, executer);
        }
        else
        {
            User *tmpUser = _server.getUserByNick(commandArgs[0]);
            if (!tmpUser)
                return ERR_NOSUCHNICK;
            std::string _msg = ":" + executer->getNickName() + " PRVMSG " + tmpUser->getNickName() + ": " + commandArgs[1];
            tmpUser->sendMessage(_msg);
        }
    }
	return SUCCESS;
}