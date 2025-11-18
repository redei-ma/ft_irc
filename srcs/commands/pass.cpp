#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include <iostream>

void	CommandHandler::passCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (executer->getIsAuthenticated())
	{
		executer->sendMessage("You are already authenticated, please try to execute a command");
		return ;
	}

	if (executer->getHasPassword())
	{
		executer->sendMessage("Your password is already set, please try to execute USER/NICK command");
		return ;
	}

	if (commandArgs.empty())
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "PASS"));

	if (commandArgs.size() > 1)
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, commandArgs[0], "PASS"));

	if (_server.getPassword() != commandArgs[0])
		return (ReplyHandler::errorHandler(ERR_PASSWDMISMATCH, *executer, commandArgs[0], "PASS"));
	else
		executer->setPassword(commandArgs[0]);

	std::cout << "PASS command executed: " << executer->getNickName() << " provided correct password" << std::endl;
	return ;
}
