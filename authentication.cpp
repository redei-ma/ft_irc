#include "CommandHandler.hpp"

bool isValidCharacher(char c)
{
	return (c=='[' || c==']' || c=='\\' || c=='`' || c=='^' || c=='{' || c=='}');
}
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

	return ;
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
