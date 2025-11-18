#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"

static void addUserToChannel(User* executer, Channel* channel)
{
	channel->addUser(executer);
	std::string welcomeMsg = ":" + executer->getNickName() + "!" + executer->getUserName() + "@irc.rfg.com" + " JOIN " + channel->getName();
	channel->broadcastMessage(welcomeMsg, executer);

	if (channel->hasTopic())
		ReplyHandler::replyHandler(RPL_TOPIC, *executer, *channel, NULL);
	
	ReplyHandler::replyHandler(RPL_NAMREPLY, *executer, *channel, NULL);
	ReplyHandler::replyHandler(RPL_ENDOFNAMES, *executer, *channel, NULL);
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
		addUserToChannel(executer, channel);
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
		else
			addUserToChannel(executer, channel);
	}
	else
		return (ReplyHandler::errorHandler(ERR_INVITEONLYCHAN, *executer, channel->getName(), "JOIN"));
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
			addUserToChannel(executer, channel);
	}
	else
	{
		//creo nuovo canale e aggiungo l user come operatore
		Channel &newChannel = _server.createChannel(inputChannel);
		newChannel.addOperator(executer);
		addUserToChannel(executer, &newChannel);
	}
	return ;
}

void	CommandHandler::joinCommand(User* executer, std::vector<std::string>& commandArgs)
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
