#include "CommandHandler.hpp"

// void CommandHandler::exitCommand(User* executer, std::vector<std::string>& commandArgs)
// {
// 	if (!executer->isRegistered())
// 	{
// 		ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "EXIT", "");
// 		return;
// 	}
// 	if (commandArgs.size()
// }

void CommandHandler::exitCommand(User* executer, std::vector<std::string>& commandArgs)
{
	(void)commandArgs;
	(void)executer;
}