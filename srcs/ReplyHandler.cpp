#include "config.hpp"
#include "ReplyHandler.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <sstream>

const std::string ReplyHandler::_prefix = ":" + std::string(SERVER_NAME); // capire se serve l'host name completo

/* ================ERROR HANDLING================ */

void ReplyHandler::errorHandler(t_status error, const User& user, const std::string& arg, const std::string& command)
{
	std::string target = user.getNickName().empty() ? "*" : user.getNickName();
	std::string buffer;

	switch (error)
	{
		case ERR_NOSUCHNICK:
			buffer = _prefix + " 401 " + target + " " + arg + " :No such nick/channel";
			break;
		case ERR_NOSUCHCHANNEL:
			buffer = _prefix + " 403 " + target + " " + arg + " :No such channel";
			break;
		case ERR_CANNOTSENDTOCHAN:
			buffer = _prefix + " 404 " + target + " " + arg + " :Cannot send to channel";
			break;
		case ERR_TOOMANYCHANNELS:
			buffer = _prefix + " 405 " + target + " " + arg + " :You have joined too many channels";
			break;
		case ERR_NOORIGIN:
			buffer = _prefix + " 409 " + target + " :No origin specified";
			break;
		case ERR_NORECIPIENT:
			buffer = _prefix + " 411 " + target + " :No recipient given (" + command + ")";
			break;
		case ERR_NOTEXTTOSEND:
			buffer = _prefix + " 412 " + target + " :No text to send";
			break;
		case ERR_UNKNOWNCOMMAND:
			buffer = _prefix + " 421 " + target + " " + command + " :Unknown command";
			break;
		case ERR_NONICKNAMEGIVEN:
			buffer = _prefix + " 431 " + target + " :No nickname given";
			break;
		case ERR_ERRONEUSNICKNAME:
			buffer = _prefix + " 432 " + target + " " + arg + " :Erroneous nickname";
			break;
		case ERR_NICKNAMEINUSE:
			buffer = _prefix + " 433 " + target + " " + arg + " :Nickname is already in use";
			break;
		case ERR_NICKCOLLISION:
			buffer = _prefix + " 436 " + target + " " + arg + " :Nickname collision";
			break;
		case ERR_USERNOTINCHANNEL:
			buffer = _prefix + " 441 " + target + " " + arg + " :They aren't on that channel";
			break;
		case ERR_NOTONCHANNEL:
			buffer = _prefix + " 442 " + target + " " + arg + " :You're not on that channel";
			break;
		case ERR_USERONCHANNEL:
			buffer = _prefix + " 443 " + target + " " + arg + " :is already on channel";
			break;
		case ERR_NOTREGISTERED:
			buffer = _prefix + " 451 " + target + " :You have not registered";
			break;
		case ERR_NEEDMOREPARAMS:
			buffer = _prefix + " 461 " + target + " " + arg + " :Not enough parameters";
			break;
		case ERR_ALREADYREGISTRED:
			buffer = _prefix + " 462 " + target + " :You may not reregister";
			break;
		case ERR_PASSWDMISMATCH:
			buffer = _prefix + " 464 " + target + " :Password incorrect";
			break;
		case ERR_KEYSET:
			buffer = _prefix + " 467 " + target + " " + arg + " :Channel key already set";
			break;
		case ERR_CHANNELISFULL:
			buffer = _prefix + " 471 " + target + " " + arg + " :Cannot join channel (+l)";
			break;
		case ERR_UNKNOWNMODE:
			buffer = _prefix + " 472 " + target + " " + arg + " :is unknown mode char to me";
			break;
		case ERR_INVITEONLYCHAN:
			buffer = _prefix + " 473 " + target + " " + arg + " :Cannot join channel (+i)";
			break;
		case ERR_BANNEDFROMCHAN:
			buffer = _prefix + " 474 " + target + " " + arg + " :Cannot join channel (+b)";
			break;
		case ERR_BADCHANNELKEY:
			buffer = _prefix + " 475 " + target + " " + arg + " :Cannot join channel (+k)";
			break;
		case ERR_NOPRIVILEGES:
			buffer = _prefix + " 481 " + target + " :Permission Denied- You're not an IRC operator";
			break;
		case ERR_CHANOPRIVSNEEDED:
			buffer = _prefix + " 482 " + target + " " + arg + " :You're not channel operator";
			break;
		case ERR_CANTKILLSERVER:
			buffer = _prefix + " 483 " + target + " :You can't kill a server!";
			break;
		case ERR_UMODEUNKNOWNFLAG:
			buffer = _prefix + " 501 " + target + " :Unknown MODE flag";
			break;
		default:
			return;
	}
	user.sendMessage(buffer);
}

// ================REPLY HANDLING================ //



void ReplyHandler::replyHandler(t_status status, const User& user, const Channel* channel, const User* targetUser)
{
	std::string buffer;

	switch (status)
	{
		case RPL_WELCOME:
			buffer = _prefix + " 001 " + user.getNickName() + " :Welcome to the Internet Relay Network " + user.getNickName() + "!" + user.getUserName() + "@" + user.getHostNameAsString();
			break;
		case RPL_YOURHOST:
			buffer = _prefix + " 002 " + user.getNickName() + " :Your host is irc.rfg.com, running version 1.0";
			break;
		case RPL_CREATED:
			buffer = _prefix + " 003 " + user.getNickName() + " :This server was created Today"; //mettere data reale
			break;
		case RPL_MYINFO:
			buffer = _prefix + " 004 " + user.getNickName() + " irc.rfg.com 1.0 - itkol";
			break;
		case RPL_CHANNELMODEIS:
			buffer = _prefix + " 324 " + user.getNickName() + " " + channel->getName() + " " + channel->getModes();
			break;
		case RPL_LIST:
		{
			size_t userCount = channel->getUserCount();
			std::stringstream ss;
			ss << userCount;
			std::string userCountStr = ss.str();
			buffer = _prefix + " 322 " + user.getNickName() + " " + channel->getName() + " " + userCountStr + " :" + channel->getTopic();
			break;
		}
		case RPL_LISTEND:
			buffer = _prefix + " 323 " + user.getNickName() + " :End of /LIST";
			break;
		case RPL_NOTOPIC:
			buffer = _prefix + " 331 " + user.getNickName() + " " + channel->getName() + " :No topic is set";
			break;
		case RPL_TOPIC:
			buffer = _prefix + " 332 " + user.getNickName() + " " + channel->getName() + " :" + channel->getTopic();
			break;
		case RPL_INVITEING:
			buffer = _prefix + " 341 " + user.getNickName() + " " + targetUser->getNickName() + " " + channel->getName();
			break;
		case RPL_NAMREPLY:
		{
			std::string namesList = "";
			const std::vector<User*>& users = channel->getUsers();
			for (std::vector<User*>::const_iterator it = users.begin(); it != users.end(); ++it)
			{
				if (channel->isOperator(*it))
					namesList += "@";
				namesList += (*it)->getNickName() + " ";
			}
			buffer = _prefix + " 353 " + user.getNickName() + " = " + channel->getName() + " :" + namesList;
			break;
		}
		case RPL_ENDOFNAMES:
			buffer = _prefix + " 366 " + user.getNickName() + " " + channel->getName() + " :End of /NAMES list";
			break;
		default:
			return;
	}
	user.sendMessage(buffer);
}
