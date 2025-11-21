#include "config.hpp"
#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"

void	CommandHandler::pingCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (executer->getIsAuthenticated() == false)
	{
		ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "PING");
		return;
	}
	if (commandArgs.size() < 1)
		 return (ReplyHandler::errorHandler(ERR_NOORIGIN, *executer, "", "PING"));

	
	std::string message = ":" + std::string(SERVER_NAME) + " PONG " + std::string(SERVER_NAME) + " :" + commandArgs[0];
	executer->sendMessage(message);

}