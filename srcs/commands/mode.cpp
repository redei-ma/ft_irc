#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <cstdlib>
#include <cerrno>
#include <algorithm>
#include <iostream>

//struttura per ogni flag
typedef struct s_flag
{
	char	flag;
	std::string	arg;
	char		sign; //puo' essere '+'/'-'
	bool		needParam;
	bool		success;
}	t_flag;

bool	isValidFlag(char& c)
{
	return (c == 'k' || c == 'i' || c == 'o' || c == 't' || c == 'l');
}

bool	checkFlags(std::vector<t_flag>& flags)
{
	if (flags.size() < 1)
		return (false);
	for (size_t i = 0; i < flags.size(); i++)
	{
		if (!isValidFlag(flags[i].flag))
			return (false);
	}
	return (true);
}

bool	flagsNeedArgs(char& c, char& sign)
{
	if (sign == '+')
		return (c == 'k' || c == 'l' || c == 'o');

	else if (sign == '-')
		return (c == 'o');

	return (false);
}

void	initFlag(t_flag& flag, char &sign, char& flagValue, size_t& needParams)
{
	flag.sign = sign;
	flag.needParam = false;
	flag.success = false;
	flag.flag = flagValue;
	flag.arg = "";
	if (flagsNeedArgs(flag.flag, flag.sign))
	{
		flag.needParam = true;
		needParams++;
	}
	else
		flag.needParam = false;
}

void	getFlags(std::string& flagsToSplit,
						std::vector<t_flag>& flags, size_t& needParams)
{
	char sign = '+';

	//parto da 1 perche':
	//[0] = canale su cui applicare le flag
	for (size_t i = 0; i < flagsToSplit.size(); i++)
	{
		//salvo la modalita' della flag
		if (flagsToSplit[i] == '+' || flagsToSplit[i] == '-')
		{
			if (flagsToSplit[i] == '+')
				sign = '+';
			else
				sign = '-';
			continue;
		}

		//creo una struttura flag con all interno sign per la flag corrente
		//e la flag corrente (anche una flag sbagliata per ora e' accettata, lo controllo dopo)
		t_flag	tmp;
		initFlag(tmp, sign, flagsToSplit[i], needParams);
		flags.push_back(tmp);
	}
}

void	getArgs(std::vector<std::string>& commandArgs,
						std::vector<t_flag>& flag, size_t& countParams)
{
	//parto da due perche 
	//[0] = canale
	//[1] = flags
	size_t argsIndex = 2;
	for (size_t i = 0; i < flag.size(); i++)
	{
		if (argsIndex < commandArgs.size() && !commandArgs[argsIndex].empty() && flag[i].needParam)
		{
			flag[i].arg = commandArgs[argsIndex++];
			countParams++;
		}
		else
			flag[i].arg = std::string("");
	}
}

void	iFlag(Channel* channel, t_flag& flag)
{
	if (flag.sign == '+')
	{
		channel->setInviteOnly(true);
		std::cout << "In channel " << channel->getName() << " invite mode is changed in invite only" << std::endl;
	}
	else
	{
		channel->setInviteOnly(false);
		std::cout << "In channel " << channel->getName() << " invite mode is changed in free mode" << std::endl;
	}

	flag.success = true;
}

void	tFlag(Channel* channel, t_flag& flag)
{
	if (flag.sign == '+')
	{
		channel->setTopicRestriction(true);
		std::cout << "In channel " << channel->getName() << " topic mode is changed in restricted" << std::endl;
	}
	else
	{
		channel->setTopicRestriction(false);
		std::cout << "In channel " << channel->getName() << " topic mode is changed in no-restricted" << std::endl;
	}

	flag.success = true;
}

void	kFlag(Channel* channel, User* executer, t_flag& flag)
{
	if (flag.sign == '+' && flag.arg.empty())
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, flag.arg, "MODE"));

	if (flag.sign == '+' && !flag.arg.empty())
	{
		if (!channel->getPassword().empty())
			return (ReplyHandler::errorHandler(ERR_KEYSET, *executer, flag.arg, "MODE"));
		channel->setPassword(flag.arg);
		std::cout << "In channel " << channel->getName() << " password mode is active" << std::endl;
	}
	else if (flag.sign == '-')
	{
		channel->removePassword();
		std::cout << "In channel " << channel->getName() << " password mode has been removed" << std::endl;
	}
	else
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, flag.arg, "MODE"));

	flag.success = true;
}

void	oFlag(Server& _server, Channel* channel, User* executer, t_flag& flag)
{
	if (flag.arg.empty())
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));

	User* user = _server.getUserByNick(flag.arg);
	if (!user || executer->getNickName() == user->getNickName())
		return (ReplyHandler::errorHandler(ERR_NOSUCHNICK, *executer, flag.arg, "MODE"));

	if (!channel->isMember(user))
		return (ReplyHandler::errorHandler(ERR_USERNOTINCHANNEL, *executer, flag.arg, "MODE"));

	if (flag.sign == '+')
	{
		channel->addOperator(user);
		std::cout << "Operator mode of " << user->getNickName() << " is changed in operator in channel" << channel->getName() << std::endl;
	}
	else
	{
		channel->removeOperator(user);
		std::cout << "Operator mode of " << user->getNickName() << " is changed in non-operator in channel" << channel->getName() << std::endl;
	}

	flag.success = true;
}

void	lFlag(Channel* channel, User* executer, t_flag& flag)
{
	if (flag.sign == '+' && !flag.arg.empty())
	{
		char* endptr;
		long result = std::strtol(flag.arg.c_str(), &endptr, 10);
		
		if (*endptr != '\0' || errno == ERANGE || result > MAX_USER || result <= 0)
			return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));
		else
		{
			channel->setUsersLimit(result);
			std::cout << "In channel " << channel->getName() << " user limit mode is active with max " << result << std::endl;
		}
	}
	else if (flag.sign == '-')
	{
		channel->removeUsersLimit();
		std::cout << "In channel " << channel->getName() << " user limit mode has been removed" << std::endl;
	}
	else
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));

	flag.success = true;
}

void	execMode(Server& _server, Channel* channel, User* executer, t_flag& flag)
{
	if (flag.flag == 'i')
		iFlag(channel, flag);
	else if (flag.flag == 't')
		tFlag(channel, flag);
	else if (flag.flag == 'k')
		kFlag(channel, executer, flag);
	else if (flag.flag == 'o')
		oFlag(_server, channel, executer, flag);
	else if (flag.flag == 'l')
		lFlag(channel, executer, flag);
}

void	CommandHandler::modeCommand(User* executer, std::vector<std::string>& commandArgs)
{
	//controllo che ci executer sia autenticato
	if (!executer->getIsAuthenticated())
		return (ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "MODE"));

	//controllo che ci sia almeno un argomento e non sia vuoto
	if (commandArgs.empty() || commandArgs[0].empty())
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));

	//controllo s il canale esiste
	Channel* channel = _server.getChannelByName(commandArgs[0]);
	if (!channel)
		return (ReplyHandler::errorHandler(ERR_NOSUCHCHANNEL, *executer, commandArgs[0], "MODE"));

	//controllo se executer e' membro del canale
	if (!channel->isMember(executer))
		return (ReplyHandler::errorHandler(ERR_NOTONCHANNEL, *executer, commandArgs[0], "MODE"));

	//controllo che il vettore di input se ho solo un argomento, do in risposta le mode del canale
	if (commandArgs.size() == 1 || commandArgs[1].empty())
	{
		ReplyHandler::replyHandler(RPL_CHANNELMODEIS, *executer, channel, NULL);
		return ;
	}

	//controllo se executer e' operatore del canale
	if (!channel->isOperator(executer))
		return (ReplyHandler::errorHandler(ERR_CHANOPRIVSNEEDED, *executer, commandArgs[0], "MODE"));

	//crep vettore di t_flag e controllo che ci siano solo flag valide
	std::vector<t_flag> flags;
	size_t	needParams = 0;
	getFlags(commandArgs[1], flags, needParams);
	if (!checkFlags(flags))
		return (ReplyHandler::errorHandler(ERR_UNKNOWNMODE, *executer, "", "MODE"));

	//aggiungo ad ogni struttura(flag) il suo argomento dove serve e conto i parametri per ogni flag che lo necessita
	size_t	countParams = 0;
	if (commandArgs.size() > 1)
		getArgs(commandArgs, flags, countParams);

	//se il numero di argomenti obbligatori e' > del numero degli argomenti che ho, errore
	if (needParams > countParams)
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "MODE"));

	//eseguo una flag alla volta
	for (size_t i = 0; i < flags.size(); i++)
	{
		execMode(_server, channel, executer, flags[i]);
		if (!flags[i].success)
			continue;
		std::string	msg = ":" + executer->getPrefix() + " MODE " +
							channel->getName() + " " + flags[i].sign + flags[i].flag;
		
		if (!flags[i].arg.empty())
			msg += " " + flags[i].arg;
		channel->broadcastMessage(msg);
	}
}
