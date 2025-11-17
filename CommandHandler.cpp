#include "CommandHandler.hpp"
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

/* ================ERROR HANDLING================ */

void CommandHandler::errorHandler(t_status error, const User& user, const std::string& arg, const std::string& command) const
{
	std::string prefix(":irc.rfg.com ");
	std::string target = user.getNickName().empty() ? "*" : user.getNickName();
	std::string buffer;

	switch (error)
	{
		case ERR_NOSUCHNICK:
			buffer = prefix + "401 " + target + " " + arg + " :No such nick/channel\r\n";
			break;
		case ERR_NOSUCHCHANNEL:
			buffer = prefix + "403 " + target + " " + arg + " :No such channel\r\n";
			break;
		case ERR_CANNOTSENDTOCHAN:
			buffer = prefix + "404 " + target + " " + arg + " :Cannot send to channel\r\n";
			break;
		case ERR_TOOMANYCHANNELS:
			buffer = prefix + "405 " + target + " " + arg + " :You have joined too many channels\r\n";
			break;
		case ERR_NORECIPIENT:
			buffer = prefix + "411 " + target + " :No recipient given (" + command + ")\r\n";
			break;
		case ERR_NOTEXTTOSEND:
			buffer = prefix + "412 " + target + " :No text to send\r\n";
			break;
		case ERR_UNKNOWNCOMMAND:
			buffer = prefix + "421 " + target + " " + command + " :Unknown command\r\n";
			break;
		case ERR_NONICKNAMEGIVEN:
			buffer = prefix + "431 " + target + " :No nickname given\r\n";
			break;
		case ERR_ERRONEUSNICKNAME:
			buffer = prefix + "432 " + target + " " + arg + " :Erroneous nickname\r\n";
			break;
		case ERR_NICKNAMEINUSE:
			buffer = prefix + "433 " + target + " " + arg + " :Nickname is already in use\r\n";
			break;
		case ERR_NICKCOLLISION:
			buffer = prefix + "436 " + target + " " + arg + " :Nickname collision\r\n";
			break;
		case ERR_USERNOTINCHANNEL:
			buffer = prefix + "441 " + target + " " + arg + " :They aren't on that channel\r\n";
			break;
		case ERR_NOTONCHANNEL:
			buffer = prefix + "442 " + target + " " + arg + " :You're not on that channel\r\n";
			break;
		case ERR_USERONCHANNEL:
			buffer = prefix + "443 " + target + " " + arg + " :is already on channel\r\n";
			break;
		case ERR_NOTREGISTERED:
			buffer = prefix + "451 " + target + " :You have not registered\r\n";
			break;
		case ERR_NEEDMOREPARAMS:
			buffer = prefix + "461 " + target + " " + arg + " :Not enough parameters\r\n";
			break;
		case ERR_PASSWDMISMATCH:
			buffer = prefix + "464 " + target + " " + arg + " :Password incorrect\r\n"; //controlla messaggio vero
			break;
		case ERR_CHANNELISFULL:
			buffer = prefix + "471 " + target + " " + arg + " :Cannot join channel (+l)\r\n";
			break;
		case ERR_INVITEONLYCHAN:
			buffer = prefix + "473 " + target + " " + arg + " :Cannot join channel (+i)\r\n";
			break;
		case ERR_BANNEDFROMCHAN:
			buffer = prefix + "474 " + target + " " + arg + " :Cannot join channel (+b)\r\n";
			break;
		case ERR_BADCHANNELKEY:
			buffer = prefix + "475 " + target + " " + arg + " :Cannot join channel (+k)\r\n";
			break;
		case ERR_NOPRIVILEGES:
			buffer = prefix + "481 " + target + " :Permission Denied- You're not an IRC operator\r\n";
			break;
		case ERR_CHANOPRIVSNEEDED:
			buffer = prefix + "482 " + target + " " + arg + " :You're not channel operator\r\n";
			break;
		case ERR_CANTKILLSERVER:
			buffer = prefix + "483 " + target + " :You can't kill a server!\r\n";
			break;
		case ERR_TOOMANYPARAMS: //controllare se puo' andare bene
			buffer = prefix + "900 " + target + " :Too many args for command " + command + "\r\n";
			break;
		case SUCCESS:
			break;
	}
	if (error != SUCCESS)
		user.sendMessage(buffer);
}

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

t_command	CommandHandler::reconizeCommand(std::string command)
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
	std::string::iterator limit = std::find(args.begin(), args.end(), ':');

	std::istringstream			stream(std::string(args.begin(), limit));
	std::string					buffer;

	while (std::getline(stream, buffer, ' '))
	{
		splittedArgs.push_back(buffer);
	}
	if (limit != args.end())
	{
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

static bool	isInvalidCharacher(char c)
{
	return (c == '-' || c == '_' || c == '.' || c == '#');
}

static bool isValidCharacher(char c)
{
	return (c == '[' || c == ']' || c == '\'' || c == '`' || c == '^' || c == '{' || c == '}');
}

t_status	CommandHandler::passCommand(User* executer, std::vector<std::string> commandArgs)
{
	if (executer->getIsAuthenticated())
	{
		executer->sendMessage("You are already authenticated, please try to execute a command");
		return (SUCCESS);
	}

	if (executer->getHasPassword())
	{
		executer->sendMessage("Your password is already set, please try to execute USER/NICK command");
		return (SUCCESS);
	}

	if (commandArgs.empty())
		return (ERR_NEEDMOREPARAMS);

	if (commandArgs.size() > 1)
		return (ERR_TOOMANYPARAMS);

	if (_server.getPassword() != commandArgs[0])
		return (ERR_PASSWDMISMATCH);
	else
		executer->setPassword(commandArgs[0]);

	return (SUCCESS);
}

t_status	CommandHandler::nickCommand(User* executer, std::vector<std::string> commandArgs)
{
	if (!executer->getHasPassword())
		return (ERR_NOTREGISTERED);

	if (commandArgs.size() == 0)
		return (ERR_NEEDMOREPARAMS);

	if (commandArgs.size() > 1)
		return (ERR_ERRONEUSNICKNAME);

	if (commandArgs[0].empty())
		return (ERR_NONICKNAMEGIVEN);

	if (commandArgs[0].size() > 9)
		return (ERR_ERRONEUSNICKNAME);

	if ((std::isdigit(commandArgs[0][0]) || isInvalidCharacher(commandArgs[0][0])) &&
			!isValidCharacher(commandArgs[0][0]))
		return (ERR_ERRONEUSNICKNAME);

	for (size_t i = 0; i < commandArgs[0].size(); i++)
	{
		if (!std::isalnum(commandArgs[0][i])
			&& !isValidCharacher(commandArgs[0][i])
			&& commandArgs[0][i] != '-')
			return (ERR_ERRONEUSNICKNAME);
	}

	if (_server.userNickEsists(commandArgs[0]))
		return (ERR_NICKNAMEINUSE);

	if (executer->getHasNickName() == true)
	{
		executer->sendMessage("NickName is already set"); // da vedere se si deve dare un errore o no
		return (SUCCESS);
	}

	executer->setNickName(commandArgs[0]);
	return SUCCESS;
}

t_status	CommandHandler::userCommand(User* executer, std::vector<std::string> commandArgs)
{
	if (!executer->getHasPassword())
		return (ERR_NOTREGISTERED);

	if (commandArgs[0].empty())
		return (ERR_NEEDMOREPARAMS);
	//:real name e' da fare????
	if (commandArgs.size() == 0)
		return (ERR_NEEDMOREPARAMS);

	if (commandArgs.size() > 1)
		return (ERR_ERRONEUSNICKNAME);

	if (commandArgs[0].size() > 9)
		return (ERR_NEEDMOREPARAMS);

	for (size_t i = 0; i < commandArgs[0].size(); i++)
	{
		if (!std::isprint(commandArgs[0][i]) && std::isspace(commandArgs[0][i]))
			return (ERR_NEEDMOREPARAMS);
	}

	if (executer->getHasUserName() == true)
	{
		executer->sendMessage("User is already set"); // da vedere se si deve dare un errore o no
		return (SUCCESS);
	}

	executer->setUserName(commandArgs[0]);
	return SUCCESS;
}

static void	SplitChannelKeys(std::vector<std::string> &channelToJoin,
							std::vector<std::string> &keys,
							const std::vector<std::string>& commandArgs)
{
	for (size_t i = 0; i < commandArgs.size(); i++)
	{
		//prendo channel con # seguendo IRC
		if (commandArgs[i][0] == '#')
			channelToJoin.push_back(commandArgs[i]);
		else
			keys.push_back(commandArgs[i]);
	}
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
		//da aggiungere altro? togliere isprint?
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

static t_status	handleWithPassword(Channel* channel, User* executer, const std::string& key)
{
	if (!key.empty() && key == channel->getPassword())
	{
		if (channel->isInviteOnly())
			channel->addInvitedUser(executer);
		else
			channel->addUser(executer);
		return (SUCCESS);
	}
	else
		return (ERR_BADCHANNELKEY);
}

static t_status	handleInviteOnly(Channel* channel, User* executer, const std::string& key)
{
	if (channel->isInvited(executer))
	{
		if (channel->hasPassword())
			return (handleWithPassword(channel, executer, key));
		channel->addInvitedUser(executer);
		return (SUCCESS);
	}
	else
		return (ERR_INVITEONLYCHAN);
}

static t_status	execJoin(Server& _server, User* executer, 
					const std::string& inputChannel, const std::string& key)
{
	Channel* channel = _server.getChannelByName(inputChannel);
	t_status	status = SUCCESS;
	if (channel != NULL)
	{
		status = canUserJoin(channel, executer);
		if (status != SUCCESS)
			return (status);

		if (channel->isInviteOnly())
			status = handleInviteOnly(channel, executer, key);
		else if (channel->hasPassword())
			status = handleWithPassword(channel, executer, key);
		else
			channel->addUser(executer);
		return (status);
	}
	else
	{
		//creo nuovo canale e aggiungo l user come operatore
		Channel &newChannel = _server.createChannel(inputChannel);
		newChannel.addUser(executer);
		newChannel.addOperator(executer);
	}
	return status;
}

t_status	CommandHandler::joinCommand(User* executer, std::vector<std::string> commandArgs)
{
	if (!executer->getIsAuthenticated())
		return (ERR_NOTREGISTERED);
	
	if (commandArgs.size() < 1 || commandArgs.size() > 15)
		return (ERR_NEEDMOREPARAMS);

	std::vector<std::string>	channelToJoin;
	std::vector<std::string>	keys;

	//riempio le due strutture splittando commandArgs
	SplitChannelKeys(channelToJoin, keys, commandArgs);

	//La key e' opzionale quindi non potranno mai essere piu dei canali
	if (keys.size() > channelToJoin.size())
		return (ERR_NEEDMOREPARAMS);

	for (size_t i = 0; i < channelToJoin.size(); i++)
	{
		if (!isValidChannelName(channelToJoin[i]))
			return (ERR_NEEDMOREPARAMS);
	}

	//Dopo aver splittato e controllato il nome di ogni canale accoppio canale e key(se c e)
	std::vector<std::pair<std::string, std::string> >	channelAndKeys;
	channelAndKeys = pairChannelAndKeys(channelToJoin, keys);

	//eseguo il comando su ogni canale e controllo esito
	for (size_t i = 0; i < channelAndKeys.size(); i++)
	{
		t_status	joinStatus = execJoin(_server, executer,
										channelAndKeys[i].first, channelAndKeys[i].second);
		if (joinStatus != SUCCESS)
			return(joinStatus);
	}
	return (SUCCESS);
}

t_status	CommandHandler::msgCommand(User* executer, std::vector<std::string> commandArgs)
{
	(void)executer;
	(void)commandArgs;
	return SUCCESS;
}

t_status	CommandHandler::kickCommand(User* executer, std::vector<std::string> commandArgs)
{
	(void)executer;
	(void)commandArgs;
	return SUCCESS;
}

t_status	CommandHandler::inviteCommand(User* executer, std::vector<std::string> commandArgs)
{
	(void)executer;
	(void)commandArgs;
	return SUCCESS;
}

t_status	CommandHandler::modeCommand(User* executer, std::vector<std::string> commandArgs)
{
	(void)executer;
	(void)commandArgs;
	return SUCCESS;
}

t_status	CommandHandler::topicCommand(User* executer, std::vector<std::string> commandArgs)
{
	// if (!executer->getIsAuthenticated())
	// 	return ERR_NOTREGISTERED;

	// if (commandArgs.size() < 1)
	// 	return ERR_NEEDMOREPARAMS;
	// else if (commandArgs.size() > 2)
	// 	return ERR_TOOMANYPARAMS;
	// else
	// {
	// 	if (!_server.channelNameExists(commandArgs[0]))
	// 		return ERR_NOSUCHCHANNEL;
	// 	Channel tmp = _server.getChannel(commandArgs[0]);

	// 	if (!tmp.isMember)
	// 		return ERR_NOTONCHANNEL;
		
	// 	if (commandArgs.size() == 1)
	// 	{
	// 		if (!tmp.hasTopic())
	// 			return RPL_NOTOPIC;
	// 		else
	// 			return RPL_TOPIC;	
	// 	}
	// 	else if (commandArgs.size() == 2)
	// 	{
	// 		if (tmp.isTopicRestricted() && !tmp.isOperator(executer))
	// 			return ERR_CHANOPRIVSNEEDED;
	// 		else
	// 		{
	// 			tmp.setTopic(commandArgs[1]);
	// 			// std::string message = ":" + executer->getNickName() + " TOPIC " + tmp.getName() + " :" + commandArgs[1] + "\r\n";
	// 			// tmp.broadcastMessage(message, executer);
	// 			return RPL_TOPIC;
	// 		}
	// 	}
	// }
	(void)executer;
	(void)commandArgs;

	return SUCCESS;
}

t_status	CommandHandler::pingCommand(User* executer, std::vector<std::string> commandArgs)
{
	(void)executer;
	(void)commandArgs;
	return SUCCESS;
}

void	CommandHandler::execCommand(User* executer, std::string input)
{
	std::vector<std::string>	splittedCommands;
	splitCommand(splittedCommands, input);

	for (size_t i = 0; i < splittedCommands.size(); i++)
	{
		std::vector<std::string> splittedArgs;
		splitArgs(splittedArgs, splittedCommands[i]);

		t_command	commandToExec = reconizeCommand(splittedArgs[0]);
		if (commandToExec == NOT_FOUND)
		{
			std::cout << splittedArgs[0] << " Comando non trovato" << std::endl;
			errorHandler(ERR_UNKNOWNCOMMAND, *executer, splittedArgs[1], splittedArgs[0]);
			continue ;
		}

		std::string	command = *splittedArgs.begin();
		splittedArgs.erase(splittedArgs.begin());

		if (splittedArgs.size() > 15)
		{
			errorHandler(ERR_NEEDMOREPARAMS, *executer, splittedArgs[0], command);
			return ;
		}

		t_status exitStatus = (this->*commandMap[commandToExec])(executer, splittedArgs);

		if (exitStatus != SUCCESS)
			errorHandler(exitStatus, *executer, splittedArgs[i], command);
	}
}

/* ================DESTRUCTOR================ */
CommandHandler::~CommandHandler() {}
