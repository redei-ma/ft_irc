#include "CommandHandler.hpp"
#include "ReplyHandler.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

/* ================MACRO================ */
#define REGISTERED_CMD(enum, name) commandMap[enum] = &CommandHandler::name##Command

/* ================CONSTRUCTOR================ */
CommandHandler::CommandHandler(Server& server) : _server(server)
{
	initCommand();
}

CommandHandler::CommandHandler(const CommandHandler& other) : _server(other._server), commandMap(other.commandMap) {}

/* ================METHODS================ */
void CommandHandler::initCommand()
{
	REGISTERED_CMD(JOIN, join);
	REGISTERED_CMD(PASS, pass);
	REGISTERED_CMD(NICK, nick);
	REGISTERED_CMD(USER, user);
	REGISTERED_CMD(PRVMSG, msg);
	REGISTERED_CMD(KICK, kick);
	REGISTERED_CMD(INVITE, invite);
	REGISTERED_CMD(TOPIC, topic);
	REGISTERED_CMD(MODE, mode);
}

t_command	CommandHandler::recognizeCommand(std::string command)
{
	const char  *commands[10] = {
		"PASS",
		"NICK",
		"USER",
		"JOIN",
		"PRVMSG",
		"KICK",
		"INVITE",
		"TOPIC",
		"MODE",
		"PING"
	};
	for (int i = 0; i < 10; i++)
	{
		if (command == commands[i])
			return static_cast<t_command>(i);
	}
	return NOT_FOUND;
}

void	splitArgs(std::vector<std::string>& splittedArgs, std::string args)
{
	//Da capire se limit deve essere ' :'
	std::string::iterator limit = std::find(args.begin(), args.end(), ':');

	std::istringstream			stream(std::string(args.begin(), limit));
	std::string					buffer;

	while (std::getline(stream, buffer, ' '))
	{
		splittedArgs.push_back(buffer);
	}
	if (limit != args.end())
	{
		//da vedere se esplode
		limit++;
		splittedArgs.push_back(std::string(limit, args.end()));
	}
}

void	splitCommand(std::vector<std::string>& splittedCommands, std::string input)
{
	std::string::size_type start = 0;
	std::string::size_type pos;

	while ((pos = input.find("\r\n", start)) != std::string::npos)
	{
		splittedCommands.push_back(input.substr(start, pos - start));
		start = pos + 2; // salta "\r\n"
	}
}

bool isValidCharacher(char c)
{
	return (c=='[' || c==']' || c=='\\' || c=='`' || c=='^' || c=='{' || c=='}');
}

void	CommandHandler::passCommand(User* executer, std::vector<std::string> commandArgs)
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
		return (ReplyHandler::errorHandler(ERR_TOOMANYPARAMS, *executer, commandArgs[0], "PASS"));

	if (_server.getPassword() != commandArgs[0])
		return (ReplyHandler::errorHandler(ERR_PASSWDMISMATCH, *executer, commandArgs[0], "PASS"));
	else
		executer->setPassword(commandArgs[0]);

	return ;
}

void	CommandHandler::nickCommand(User* executer, std::vector<std::string> commandArgs)
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

void	CommandHandler::userCommand(User* executer, std::vector<std::string> commandArgs)
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

static bool	SplitChannelKeys(std::vector<std::string> &channelToJoin,
							std::vector<std::string> &keys,
							const std::vector<std::string>& commandArgs)
{
	bool	finishedChannels = false;
	for (size_t i = 0; i < commandArgs.size(); i++)
	{
		//prendo channel con # seguendo IRC
		if (commandArgs[i][0] == '#' && !finishedChannels)
		{
			channelToJoin.push_back(commandArgs[i]);
		}
		else
		{
			finishedChannels = true;
			if (commandArgs[i][0] == '#')
				return (false);
			keys.push_back(commandArgs[i]);
		}
	}
	return (true);
}

static bool	isValidChannelName(const std::string& channel)
{
	if (channel.empty() || channel.size() == 1 || channel.size() > 15)
		return (false);
	
	if (channel[0] != '#')
		return (false);

	// salto #
	for (size_t i = 1; i < channel.size(); i++)
	{
		if (std::isspace(channel[i]) || channel[i] == '#' ||
			channel[i] == ',' || channel[i] == ':' || !std::isprint(channel[i]))
				return (false);
	}
	return (true);
}

static std::vector<std::pair
	<std::string, std::string> > pairChannelAndKeys(const std::vector<std::string>& channelToJoin,
													const std::vector<std::string>& keys)
{
	std::vector<std::pair<std::string, std::string> > channelAndKeys;

	for (size_t i = 0; i < channelToJoin.size(); i++)
	{
		if (!keys.empty())
			channelAndKeys.push_back(std::make_pair(channelToJoin[i], keys[i]));
		//accoppio una stringa vuota per facilitare i controlli dopo
		else
			channelAndKeys.push_back(std::make_pair(channelToJoin[i], std::string("")));
	}
	return (channelAndKeys);
}

static t_status	canUserJoin(Channel* channel, User* executer)
{
	//controllo che l user non sia gia' nel canale
	if (channel->isMember(executer))
		return (ERR_USERONCHANNEL);

	//controllo se il canale è pieno
	if (channel->isFull())
		return (ERR_CHANNELISFULL);
	
	return (SUCCESS);
}

static bool	handleWithPassword(Channel* channel, User* executer, const std::string& key)
{
	if (!key.empty() && key == channel->getPassword())
	{
		channel->addUser(executer);
		return true;
	}

	ReplyHandler::errorHandler(ERR_BADCHANNELKEY, *executer, channel->getName(), "JOIN");
	return (false);
}

static void	handleInviteOnly(Channel* channel, User* executer, const std::string& key)
{
	if (channel->isInvited(executer))
	{
		if (channel->hasPassword())
		{
			if (!handleWithPassword(channel, executer, key))
				return ;
		}
		channel->addUser(executer);
		if (channel->hasTopic())
			return ReplyHandler::replyHandler(RPL_TOPIC, );
		else
			return ReplyHandler::replyHandler(RPL_NOTOPIC, );
	}
	else
		return (ReplyHandler::errorHandler(ERR_INVITEONLYCHAN, *executer, channel->getName(), "JOIN") );
}

static void	execJoin(Server& _server, User* executer, 
					const std::string& inputChannel, const std::string& key)
{
	Channel* channel = _server.getChannelByName(inputChannel);

	t_status	status = SUCCESS;

	if (channel != NULL)
	{
		status = canUserJoin(channel, executer);
		if (status != SUCCESS)
			return (ReplyHandler::errorHandler(status, *executer, inputChannel, "JOIN"));

		if (channel->isInviteOnly())
			handleInviteOnly(channel, executer, key);
		else if (channel->hasPassword())
			handleWithPassword(channel, executer, key);
		else
			channel->addUser(executer);
	}
	else
	{
		//creo nuovo canale e aggiungo l user come operatore
		Channel &newChannel = _server.createChannel(inputChannel);
		newChannel.addUser(executer);
		newChannel.addOperator(executer);
	}
	return ;
}

void	CommandHandler::joinCommand(User* executer, std::vector<std::string> commandArgs)
{
	if (!executer->getIsAuthenticated())
		return (ReplyHandler::errorHandler(ERR_NOTREGISTERED, *executer, "", "JOIN"));

	if (commandArgs.size() < 1 || commandArgs.size() > 15)
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "JOIN"));

	std::vector<std::string>	channelToJoin;
	std::vector<std::string>	keys;

	//riempio le due strutture splittando commandArgs
	if (!SplitChannelKeys(channelToJoin, keys, commandArgs))
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, channelToJoin[0], "JOIN"));

	//La key e' opzionale quindi non potranno mai essere piu dei canali
	if (keys.size() > channelToJoin.size())
		return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "", "JOIN"));

	for (size_t i = 0; i < channelToJoin.size(); i++)
	{
		if (!isValidChannelName(channelToJoin[i]))
			return (ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, channelToJoin[i], "JOIN"));
	}

	//Dopo aver splittato e controllato il nome di ogni canale accoppio canale e key(se c e)
	std::vector<std::pair<std::string, std::string> >	channelAndKeys;
	channelAndKeys = pairChannelAndKeys(channelToJoin, keys);

	//eseguo il comando su ogni canale
	for (size_t i = 0; i < channelAndKeys.size(); i++)
	{	
		execJoin(_server, executer,
				channelAndKeys[i].first, channelAndKeys[i].second);
	}
	return ;
}

void	CommandHandler::msgCommand(User* executer, std::vector<std::string> commandArgs)
{
	(void)executer;
	(void)commandArgs;
	return ;
}

void	CommandHandler::kickCommand(User* executer, std::vector<std::string> commandArgs)
{
	if (!executer->getIsAuthenticated())
		return ERR_NOTREGISTERED;

	if (commandArgs.size() < 2 || commandArgs.size() > 3)
		return ERR_NEEDMOREPARAMS;
	
	Channel *channel = _server.getChannelByName(commandArgs[0]);
	if (!channel)
		return ERR_NOSUCHCHANNEL;

	if (!channel->isMember(executer))
		return ERR_NOTONCHANNEL;
	
	if (!channel->isOperator(executer))
		return ERR_CHANOPRIVSNEEDED;
	
	User* targetUser = _server.getUserByNick(commandArgs[1]);
	if (!targetUser)
		return ERR_NOSUCHNICK;

	if (!channel->isMember(targetUser))
		return ERR_USERNOTINCHANNEL;

	std::string reason;
	if (commandArgs.size() == 3)
		reason = " :" + commandArgs[2];
	else
		reason = "";
	std::string message = ":" + executer->getNickName() + " KICK " + channel->getName() + " " + targetUser->getNickName() + reason + "\r\n";
	channel->broadcastMessage(message, NULL);

	channel->removeUser(targetUser);
	// return RPL_INVITING;

	return ;
}

void	CommandHandler::inviteCommand(User* executer, std::vector<std::string> commandArgs)
{
	if (!executer->getIsAuthenticated())
		return ERR_NOTREGISTERED;

	if (commandArgs.size() != 2)
		return ERR_NEEDMOREPARAMS;

	User* invitedUser = _server.getUserByNick(commandArgs[0]);
	if (!invitedUser)
		return ERR_NOSUCHNICK;

	Channel *channel = _server.getChannelByName(commandArgs[1]);
	if (!channel)
		return ERR_NOSUCHCHANNEL;

	if (!channel->isMember(executer))
		return ERR_NOTONCHANNEL;

	if (channel->isMember(invitedUser))
		return ERR_USERONCHANNEL;

	if (channel->isInviteOnly() && !channel->isOperator(executer))
		return ERR_CHANOPRIVSNEEDED;

	channel->inviteUser(invitedUser);
	invitedUser->sendMessage(":" + executer->getNickName() + " INVITE " + invitedUser->getNickName() + " :" + channel->getName() + "\r\n");

	std::string message = ":irc.rfg.com 341 " + executer->getNickName() + " " + invitedUser->getNickName() + " " + channel->getName() + "\r\n";
	executer->sendMessage(message);
	// return RPL_INVITING;

	return ;
}

void	CommandHandler::modeCommand(User* executer, std::vector<std::string> commandArgs)
{
	(void)executer;
	(void)commandArgs;
}

void	CommandHandler::topicCommand(User* executer, std::vector<std::string> commandArgs)
{
	if (!executer->getIsAuthenticated())
		return ERR_NOTREGISTERED;

	if (commandArgs.size() < 1 || commandArgs.size() > 2)
		return ERR_NEEDMOREPARAMS;
	else
	{
		Channel *channel = _server.getChannelByName(commandArgs[0]);
		if (!channel)
			return ERR_NOSUCHCHANNEL;

		if (!channel->isMember(executer))
			return ERR_NOTONCHANNEL;

		if (commandArgs.size() == 1)
		{
			if (!channel->hasTopic())
			{
				std::string message = ":irc.rfg.com 331 " + executer->getNickName() + " " + channel->getName() + " :No topic is set\r\n";
				executer->sendMessage(message);
				// return RPL_NOTOPIC;
			}
			else 
			{
				std::string message = ":irc.rfg.com 332 " + executer->getNickName() + " " + channel->getName() + " :" + channel->getTopic() + "\r\n";
				executer->sendMessage(message);
				// return RPL_TOPIC;
			}
		}
		else if (commandArgs.size() == 2)
		{
			if (channel->isTopicRestricted() && !channel->isOperator(executer))
				return ERR_CHANOPRIVSNEEDED;
			else
			{
				channel->setTopic(commandArgs[1]);
				std::string message = ":" + executer->getNickName() + " TOPIC " + channel->getName() + " :" + channel->getTopic() + "\r\n";
				channel->broadcastMessage(message, NULL);
				// return RPL_TOPIC;
			}
		}
	}
	return ;
}

void	CommandHandler::pingCommand(User* executer, std::vector<std::string> commandArgs)
{
	(void)executer;
	(void)commandArgs;
	return ;
}

void	CommandHandler::execCommand(User* executer, std::string input)
{
	std::vector<std::string>	splittedCommands;
	splitCommand(splittedCommands, input);

	for (size_t i = 0; i < splittedCommands.size(); i++)
	{
		std::vector<std::string> splittedArgs;
		splitArgs(splittedArgs, splittedCommands[i]);

		t_command	commandToExec = recognizeCommand(splittedArgs[0]);
		if (commandToExec == NOT_FOUND)
		{
			std::cout << splittedArgs[0] << " Comando non trovato" << std::endl;
			ReplyHandler::errorHandler(ERR_UNKNOWNCOMMAND, *executer, splittedArgs[1], splittedArgs[0]);
			continue ;
		}

		std::string	command = *splittedArgs.begin();
		splittedArgs.erase(splittedArgs.begin());

		(this->*commandMap[commandToExec])(executer, splittedArgs);
	}
}

/* ================DESTRUCTOR================ */
CommandHandler::~CommandHandler() {}
