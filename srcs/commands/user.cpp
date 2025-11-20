#include "CommandHandler.hpp"
#include "User.hpp"
#include "utils.hpp"
#include <iostream>

void	CommandHandler::userCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (!executer->getHasPassword())
		return (ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "USER"));

	if (!executer->getHasNickName())
		return (ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "USER"));

	if (commandArgs.size() > 4)
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "USER"));

	if (commandArgs[0].empty() || commandArgs[0].size() > 9)
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, commandArgs[0], "USER"));

	for (size_t i = 0; i < commandArgs[0].size(); i++)
	{
		if (!std::isprint(commandArgs[0][i]) || std::isspace(commandArgs[0][i]))
			return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, commandArgs[0], "USER"));
	}

	if (executer->getHasUserName() == true)
	{
		std::string msg = ":" + executer->getNickName() + "!" +
						executer->getUserName() + "@" + executer->getHostNameAsString() + " USER " + " :" + commandArgs[0];

		executer->sendMessage(msg);
		return ;
	}

	executer->setUserName(commandArgs[0]);

	if (!commandArgs[4].empty())
		executer->setRealName(commandArgs[4]);

	ReplyHandler::replyHandler(RPL_WELCOME, *executer, NULL, NULL);
	ReplyHandler::replyHandler(RPL_YOURHOST, *executer, NULL, NULL);
	ReplyHandler::replyHandler(RPL_CREATED, *executer, NULL, NULL);
	ReplyHandler::replyHandler(RPL_MYINFO, *executer, NULL, NULL);

	std::cout << "USER command executed: " << executer->getNickName() << " set username" << std::endl;
	return ;
}
