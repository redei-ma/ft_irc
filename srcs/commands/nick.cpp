#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"

static bool isValidCharacher(char c)
{
	return (c=='[' || c==']' || c=='\\' || c=='`' || c=='^' || c=='{' || c=='}');
}

void	CommandHandler::nickCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (!executer->getHasPassword())
		return (ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "NICK"));

	if (commandArgs.size() != 1)
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "NICK"));

	if (commandArgs[0].empty())
		return (ReplyHandler::errorHandler(ERR_NONICKNAMEGIVEN, *executer, "", "NICK"));

	if (commandArgs[0].size() > 9)
		return (ReplyHandler::errorHandler(ERR_ERRONEUSNICKNAME, *executer, commandArgs[0], "NICK"));

	if (!std::isalpha(commandArgs[0][0]) && !isValidCharacher(commandArgs[0][0]))
		return (ReplyHandler::errorHandler(ERR_ERRONEUSNICKNAME, *executer, commandArgs[0], "NICK"));

	for (size_t i = 0; i < commandArgs[0].size(); i++)
	{
		if (!std::isalnum(commandArgs[0][i]) && !isValidCharacher(commandArgs[0][i]) && commandArgs[0][i] != '-')
			return (ReplyHandler::errorHandler(ERR_ERRONEUSNICKNAME, *executer, commandArgs[0], "NICK"));
	}

	if (_server.userNickEsists(commandArgs[0]))
		return (ReplyHandler::errorHandler(ERR_NICKNAMEINUSE, *executer, commandArgs[0], "NICK"));

	if (executer->getHasNickName() == true)
	{
		executer->sendMessage("NickName is already set"); // da vedere se si deve dare un errore o no
		return ;
	}

	executer->setNickName(commandArgs[0]);
	return ;
}
