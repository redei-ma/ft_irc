#include "CommandHandler.hpp"
#include "ReplyHandler.hpp"
#include "User.hpp"
#include <algorithm>
#include <climits>
#include <cstddef>

bool	isValidFlag(std::string& c)
{
	return (c == "k" || c == "i" || c == "o" || c == "t" || c == "l");
}

bool	checkFlags(std::vector<std::string>& flags)
{
	for (size_t i = 0; i < flags.size(); i++)
	{
		if (!isValidFlag(flags[i]))
			return (false);
	}
	return (true);
}

void	getFlags(std::string& flagsToSplit,
						std::vector<std::string>& flags, int& needParams, bool& modeFlag)
{
	for (size_t i = 1; i < flagsToSplit.size(); i++)
	{
		if (flagsToSplit[i] == '+' || flagsToSplit[i] == '-')
			continue;

		//+k +l +o devono avere argomento
		if ((flagsToSplit[i] == 'k' || flagsToSplit[i] == 'l' || flagsToSplit[i] == 'o') && modeFlag)
			needParams++;

		//-o e' l unica flag che e' con argomento obbligatorio
		else if (flagsToSplit[i] == 'o' && !modeFlag)
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

bool	flagsNeedArgs(std::string& c, bool& modeFlag)
{
	if (modeFlag)
		return (c == "k" || c == "l" || c == "o");

	else if (!modeFlag)
		return (c == "o");

	return false;
}

bool	pairFlagsAndArgs(std::vector<std::string>& flags, std::vector<std::string>& args,
						std::vector<std::pair<std::string, std::string> >& flagsAndArgs, bool& modeFlag)
{
	int	argsIndex = 0;

	for (size_t i = 0; i < flags.size(); i++)
	{
		if (flagsNeedArgs(flags[i], modeFlag))
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

void	iFlag(Channel* channel, User* executer, std::string arg, bool modeFlag)
{
	if (modeFlag)
		channel->setInviteOnly(true);
	else
		channel->setInviteOnly(false);
}

void	tFlag(Channel* channel, User* executer, std::string arg, bool modeFlag)
{
	if (modeFlag)
		channel->setTopicRestriction(true);
	else
		channel->setTopicRestriction(false);
}

void	kFlag(Channel* channel, User* executer, std::string arg, bool modeFlag)
{
	if (modeFlag && !arg.empty())
	{
		if (!channel->getPassword().empty())
			return (ReplyHandler::errorHandler(ERR_KEYSET, *executer, arg, "MODE"));
		channel->setPassword(arg);
	}
	else
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, arg, "MODE"));

	if (!modeFlag)
		channel->removePassword();
}

void	oFlag(Server& _server, Channel* channel, User* executer, std::string arg, bool modeFlag)
{
	if (arg.empty())
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));

	User* user = _server.getUserByNick(arg);
	if (!user)
		return (ReplyHandler::errorHandler(ERR_NOSUCHNICK, *executer, arg, "MODE"));

	if (!channel->isMember(user))
		return (ReplyHandler::errorHandler(ERR_USERNOTINCHANNEL, *executer, arg, "MODE"));

	if (modeFlag)
		channel->addOperator(user);
	else
		channel->removeOperator(user);
}

void	lFlag(Channel* channel, User* executer, std::string arg, bool modeFlag)
{
	if (modeFlag && !arg.empty())
	{
		char* endptr;
		long result = std::strtol(arg.c_str(), &endptr, 10);
		
		if (*endptr != '\0' || errno == ERANGE || result > MAX_USR_NBR || result <= 0)
			return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));
		else
			channel->setUsersLimit(result);
	}
	else
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));

	if (!modeFlag)
		channel->removeUsersLimit();
}

void	execMode(Server& _server, Channel* channel, User* executer, std::string flag, std::string arg, bool modeFlag)
{
	if (flag == "i")
		iFlag(channel, executer, arg, modeFlag);
	else if (flag == "t")
		tFlag(channel, executer, arg, modeFlag);
	else if (flag == "k")
		kFlag(channel, executer, arg, modeFlag);
	else if (flag == "o")
		oFlag(_server, channel, executer, arg, modeFlag);
	else if (flag == "l")
		lFlag(channel, executer, arg, modeFlag);
	else
		ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, flag, "MODE"); // da vedere che errore sostituire
}

void	CommandHandler::modeCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (executer->getIsAuthenticated())
		return (ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "MODE"));
	
	if (commandArgs.empty())
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));

	if (commandArgs[0].empty())
		return (ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, "", "MODE")); // ridondante con controllo sotto?

	int	countChannel = std::count(commandArgs.begin(), commandArgs.end(), '#');
	if (countChannel == 0 || countChannel > 1)
		return (ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, "", "MODE"));

	Channel* channel = _server.getChannelByName(commandArgs[0]);
	if (!channel)
		return (ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, commandArgs[0], "MODE"));

	if (!channel->isMember(executer))
		return (ReplyHandler::errorHandler(ERR_NOTONCHANNEL, *executer, commandArgs[0], "MODE"));

	if (!channel->isOperator(executer))
		return (ReplyHandler::errorHandler(ERR_CHANOPRIVSNEEDED, *executer, commandArgs[0], "MODE"));

	if (commandArgs.size() == 1 || commandArgs[1].empty())
	{
		ReplyHandler::replyHandler(RPL_CHANNELMODEIS, *executer, *channel, NULL);
		return ;
	}

	int	positiveSign = std::count(commandArgs[1].begin(), commandArgs[1].end(), '+');
	int	negativeSign = std::count(commandArgs[1].begin(), commandArgs[1].end(), '-');

	bool	modeFlag = false;

	if ((positiveSign > 0 && negativeSign > 0) || (positiveSign == 0 && negativeSign == 0))
		return (ReplyHandler::errorHandler(ERR_UNKNOWNMODE, *executer, commandArgs[0], "MODE"));
	else if (positiveSign > 0)
		modeFlag = true;

	std::vector<std::string> flags;
	int	needParams = 0;
	getFlags(commandArgs[1], flags, needParams, modeFlag);
	if (!checkFlags(flags))
		return (ReplyHandler::errorHandler(ERR_UNKNOWNMODE, *executer, "", "MODE"));

	std::vector<std::string> args;
	if (commandArgs.size() > 2)
		getArgs(commandArgs, args);

	if (needParams > args.size())
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));

	std::vector<std::pair<std::string, std::string> > flagsAndArgs;
	if (!pairFlagsAndArgs(flags, args, flagsAndArgs, modeFlag))
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));

	for (size_t i = 0; i < flagsAndArgs.size(); i++)
	{
		execMode(_server, channel, executer, flagsAndArgs[i].first, flagsAndArgs[i].second, modeFlag);
	}
}
