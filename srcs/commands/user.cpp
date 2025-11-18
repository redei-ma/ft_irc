#include "CommandHandler.hpp"

void	CommandHandler::userCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (!executer->getHasPassword())
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "USER"));

	//:real name e' da fare????

	if (commandArgs.size() != 1)
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
		executer->sendMessage("User is already set"); // da vedere se si deve dare un errore o no
		return ;
	}

	executer->setUserName(commandArgs[0]);
	return ;
}
