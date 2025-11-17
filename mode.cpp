#include "CommandHandler.hpp"
#include "ReplyHandler.hpp"
#include <algorithm>
#include <cstddef>

void	getFlags(std::string& flagsToSplit,
						std::vector<std::string>& flags, int& needParams)
{
	for (size_t i = 1; i < flagsToSplit.size(); i++)
	{
		if (flagsToSplit[i] == '+' || flagsToSplit[i] == '-')
			continue;

		if (flagsToSplit[i] == 'k' || flagsToSplit[i] == 'l' || flagsToSplit[i] == 'o')
			needParams++;
		flags.push_back(std::string(1, flagsToSplit[i]));
	}
}

void	getArgs(std::vector<std::string>& commandArgs,
						std::vector<std::string>& args)
{
	for (size_t i = 2; i < commandArgs.size(); i++)
	{
		args.push_back(commandArgs[i]);
	}
}

bool	pairFlagsAndArgs(std::vector<std::string>& flags, std::vector<std::string>& args,
						std::vector<std::pair<std::string, std::string> >& flagsAndArgs)
{
	int	argsIndex = 0;

	for (size_t i = 0; i < flags.size(); i++)
	{
		if ((flags[i] == "k" || flags[i] == "l" || flags[i] == "o"))
		{
			if (argsIndex < args.size())
				flagsAndArgs.push_back(std::make_pair(flags[i], args[argsIndex++]));
			else
				return (false);
		}
		else
			flagsAndArgs.push_back(std::make_pair(flags[i], std::string("")));
	}
	return (true);
}

void	execIflag(Channel* channel, User* executer, std::string arg, bool modeFlag)
{

}

void	execTflag(Channel* channel, User* executer, std::string arg, bool modeFlag)
{

}

void	execKflag(Channel* channel, User* executer, std::string arg, bool modeFlag)
{

}

void	execOflag(Channel* channel, User* executer, std::string arg, bool modeFlag)
{

}

void	execLflag(Channel* channel, User* executer, std::string arg, bool modeFlag)
{

}

void	execMode(Channel* channel, User* executer, std::string flag, std::string arg, bool modeFlag)
{
	if (flag == "i")
		execIflag(channel, executer, arg, modeFlag);
	else if (flag == "t")
		execTflag(channel, executer, arg, modeFlag);
	else if (flag == "k")
		execKflag(channel, executer, arg, modeFlag);
	else if (flag == "o")
		execOflag(channel, executer, arg, modeFlag);
	else if (flag == "l")
		execLflag(channel, executer, arg, modeFlag);
	else
		ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, flag, "MODE"); // da vedere che errore sostituire
}

void	CommandHandler::modeCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (commandArgs.empty())
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));

	if (commandArgs[0].empty())
		return (ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, "", "MODE"));

	int	countChannel = std::count(commandArgs.begin(), commandArgs.end(), '#');
	if (countChannel == 0 || countChannel > 1)
		return (ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, "", "MODE"));

	Channel* channel = _server.getChannelByName(commandArgs[0]);
	if (!channel)
		return (ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, commandArgs[0], "MODE"));

	if (commandArgs.size() == 1 || commandArgs[1].empty())
	{
		ReplyHandler::replyHandler(RPL_CHANNELMODEIS, *executer, *channel, NULL);
		return ;
	}

	int	positiveSign = std::count(commandArgs[1].begin(), commandArgs[1].end(), '+');
	int	negativeSign = std::count(commandArgs[1].begin(), commandArgs[1].end(), '-');

	bool	modeFlag = false;

	if ((positiveSign > 0 && negativeSign > 0) || (positiveSign == 0 && negativeSign == 0))
		return (ReplyHandler::errorHandler(ERR_UMODEUNKNOWNFLAG, *executer, commandArgs[0], "MODE"));//ERR_UMODEUNKNOWNFLAG DA AGGIUNGERE
	else if (positiveSign > 0)
		modeFlag = true;

	std::vector<std::string> flags;
	int	needParams = 0;
	getFlags(commandArgs[1], flags, needParams);

	std::vector<std::string> args;
	if (commandArgs.size() > 2)
		getArgs(commandArgs, args);

	if (needParams > args.size())
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));

	std::vector<std::pair<std::string, std::string> > flagsAndArgs;
	if (!pairFlagsAndArgs(flags, args, flagsAndArgs))
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));

	for (size_t i = 0; i < flagsAndArgs.size(); i++)
	{
		execMode(channel, executer, flagsAndArgs[i].first, flagsAndArgs[i].second, modeFlag);
	}
}