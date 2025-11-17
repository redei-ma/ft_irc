#ifndef REPLYHANDLER_HPP
# define REPLYHANDLER_HPP

class User;
# include <string>

typedef enum	s_status
{
	SUCCESS = 0,
	ERR_NOSUCHNICK = 401,		//<nick> :No such nick/channel	Il nickname o canale specificato non esiste.
	//ERR_NOSUCHSERVER = 402,		//<server> :No such server	Server non trovato.
	ERR_NOSUCHCHANNEL = 403,	//<channel> :No such channel	Canale non esistente.
	ERR_CANNOTSENDTOCHAN = 404,	//<channel> :Cannot send to channel	Il client non può inviare messaggi al canale.
	ERR_TOOMANYCHANNELS = 405,	//<channel> :You have joined too many channels	Limite di canali raggiunto per un utente.
	ERR_NORECIPIENT = 411,		//No recipient given (<command>)	Comando senza destinatario.
	ERR_NOTEXTTOSEND = 412,		//No text to send	Messaggio vuoto da inviare.
	ERR_UNKNOWNCOMMAND = 421,	//Unknown command	Comando non riconosciuto.
	ERR_NONICKNAMEGIVEN = 431,	//No nickname given	Nessun nickname fornito con NICK.
	ERR_ERRONEUSNICKNAME = 432,	//Erroneous nickname	Nickname invalido.
	ERR_NICKNAMEINUSE = 433,	//Nickname is already in use	Nickname già utilizzato da un altro utente.
	ERR_NICKCOLLISION = 436,	//Nickname collision	Collisione tra nickname (rare).
	ERR_USERNOTINCHANNEL = 441,	//They aren't on that channel	L’utente non è nel canale.
	ERR_NOTONCHANNEL = 442,		//You're not on that channel	Tentativo di operazione su un canale dove l’utente non è presente.
	ERR_USERONCHANNEL = 443,	//is already on channel	L’utente è già nel canale.
	ERR_NOTREGISTERED = 451,	//You have not registered	Il client non ha completato la registrazione.
	ERR_NEEDMOREPARAMS = 461,	//Not enough parameters	Parametri insufficienti per il comando.
	ERR_PASSWDMISMATCH = 464,	
	ERR_CHANNELISFULL = 471,	//Cannot join channel (+l)	Canale pieno.
	ERR_INVITEONLYCHAN = 473,	//Cannot join channel (+i)	Canale solo su invito.
	ERR_BANNEDFROMCHAN = 474,	//Cannot join channel (+b)	L’utente è bannato dal canale.
	ERR_BADCHANNELKEY = 475,	//Cannot join channel (+k)	Password del canale errata.
	ERR_NOPRIVILEGES = 481,		//Permission Denied- You're not an IRC operator	Comando riservato agli operatori.
	ERR_CHANOPRIVSNEEDED = 482,	//You're not channel operator	L’utente non è operatore del canale.
	ERR_CANTKILLSERVER = 483,	//You can't kill a server!	Comando KILL verso un server.
	ERR_TOOMANYPARAMS = 900,

	//DA METTERE NEL SERVER???
	//RPL_YOURHOST = 002,			//Your host is <servername>	Info sul server.
	//RPL_CREATED = 003,			//This server was created <date>	Data di creazione server.
	//RPL_MYINFO = 004,			//<servername> <version> <available user modes> <available channel modes>	Info sul server e modalità

}	t_status;


class ReplyHandler
{
	public:
			static void	errorHandler(t_status, const User&, const std::string&, const std::string&);
			static void	replyHandler(t_status, const User&, const std::string&, const std::string&);
};

#endif
