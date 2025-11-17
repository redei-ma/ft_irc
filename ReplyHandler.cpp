#include "ReplyHandler.hpp"
#include "User.hpp"


/* ================ERROR HANDLING================ */

void ReplyHandler::errorHandler(t_status error, const User& user, const std::string& arg, const std::string& command)
{
	std::string prefix(":irc.rfg.com ");
	std::string target = user.getNickName().empty() ? "*" : user.getNickName();
	std::string buffer;

	switch (error)
	{
		case ERR_NOSUCHNICK:
			buffer = prefix + "401 " + target + " " + arg + " :No such nick/channel";
			break;
		case ERR_NOSUCHCHANNEL:
			buffer = prefix + "403 " + target + " " + arg + " :No such channel";
			break;
		case ERR_CANNOTSENDTOCHAN:
			buffer = prefix + "404 " + target + " " + arg + " :Cannot send to channel";
			break;
		case ERR_TOOMANYCHANNELS:
			buffer = prefix + "405 " + target + " " + arg + " :You have joined too many channels";
			break;
		case ERR_NORECIPIENT:
			buffer = prefix + "411 " + target + " :No recipient given (" + command + ")";
			break;
		case ERR_NOTEXTTOSEND:
			buffer = prefix + "412 " + target + " :No text to send";
			break;
		case ERR_UNKNOWNCOMMAND:
			buffer = prefix + "421 " + target + " " + command + " :Unknown command";
			break;
		case ERR_NONICKNAMEGIVEN:
			buffer = prefix + "431 " + target + " :No nickname given";
			break;
		case ERR_ERRONEUSNICKNAME:
			buffer = prefix + "432 " + target + " " + arg + " :Erroneous nickname";
			break;
		case ERR_NICKNAMEINUSE:
			buffer = prefix + "433 " + target + " " + arg + " :Nickname is already in use";
			break;
		case ERR_NICKCOLLISION:
			buffer = prefix + "436 " + target + " " + arg + " :Nickname collision";
			break;
		case ERR_USERNOTINCHANNEL:
			buffer = prefix + "441 " + target + " " + arg + " :They aren't on that channel";
			break;
		case ERR_NOTONCHANNEL:
			buffer = prefix + "442 " + target + " " + arg + " :You're not on that channel";
			break;
		case ERR_USERONCHANNEL:
			buffer = prefix + "443 " + target + " " + arg + " :is already on channel";
			break;
		case ERR_NOTREGISTERED:
			buffer = prefix + "451 " + target + " :You have not registered";
			break;
		case ERR_NEEDMOREPARAMS:
			buffer = prefix + "461 " + target + " " + arg + " :Not enough parameters";
			break;
		case ERR_PASSWDMISMATCH:
			buffer = prefix + "464 " + target + " " + arg + " :Password incorrect"; //controlla messaggio vero
			break;
		case ERR_CHANNELISFULL:
			buffer = prefix + "471 " + target + " " + arg + " :Cannot join channel (+l)";
			break;
		case ERR_INVITEONLYCHAN:
			buffer = prefix + "473 " + target + " " + arg + " :Cannot join channel (+i)";
			break;
		case ERR_BANNEDFROMCHAN:
			buffer = prefix + "474 " + target + " " + arg + " :Cannot join channel (+b)";
			break;
		case ERR_BADCHANNELKEY:
			buffer = prefix + "475 " + target + " " + arg + " :Cannot join channel (+k)";
			break;
		case ERR_NOPRIVILEGES:
			buffer = prefix + "481 " + target + " :Permission Denied- You're not an IRC operator";
			break;
		case ERR_CHANOPRIVSNEEDED:
			buffer = prefix + "482 " + target + " " + arg + " :You're not channel operator";
			break;
		case ERR_CANTKILLSERVER:
			buffer = prefix + "483 " + target + " :You can't kill a server!";
			break;
		case ERR_TOOMANYPARAMS: //controllare se puo' andare bene
			buffer = prefix + "900 " + target + " :Too many args for command " + command;
			break;
		case SUCCESS:
			break;
	}
	if (error != SUCCESS)
		user.sendMessage(buffer);
}