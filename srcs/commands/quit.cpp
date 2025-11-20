#include "CommandHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <iostream>

// mon so se controllo due volete i canali e ci esco due volte

void CommandHandler::quitCommand(User* executer, std::vector<std::string>& commandArgs)
{
	if (commandArgs.size() > 1)
	{
		ReplyHandler::errorHandler(ERR_NEEDMOREPARAMS, *executer, "QUIT", "");
		return;
	}

	std::string quitMessage = (commandArgs.empty()) ? "Client exiting" : commandArgs[0];

	// Notifica a tutti i Canali
	std::vector<Channel *>& userChannels = executer->getChannelVector();
	std::string message = executer->getPrefix() + " QUIT :" + quitMessage;
	for (size_t i = 0; i < userChannels.size(); i++)
	{
		Channel *channel = userChannels[i];
		channel->broadcastMessage(message);
		channel->removeUser(executer);
		if (channel->getUserCount() == 0)
			_server.deleteChannel(channel);
	}

	std::cout << "QUIT command executed: " << executer->getNickName() << " disconnected from server." << std::endl;
	// Richiesta di Disconnessione
	_server.disconnectUser(executer);

	return;
}