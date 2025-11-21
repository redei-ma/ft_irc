#ifndef REPLYHANDLER_HPP
# define REPLYHANDLER_HPP

# include <string>

class User;
class Channel;

typedef enum	s_status
{
	SUCCESS = 0,

	RPL_WELCOME = 001,			//Welcome to the Internet Relay Network <nick>!<user>@<host
	RPL_YOURHOST = 002,			//Your host is <servername>	Info sul server.
	RPL_CREATED = 003,			//This server was created <date>	Data di creazione server.
	RPL_MYINFO = 004,			//<servername> <version> <available user modes> <available channel modes>	Info sul server e modalità
	RPL_LIST = 322,				//<channel> <# visible> :<topic>	Elenco canali.
	RPL_LISTEND = 323,			//:End of /LIST	Termine
	RPL_CHANNELMODEIS = 324,    // "<channel> <modes> [params]"
	RPL_NOTOPIC = 331,			//<channel> :No topic is set	Canale senza topic.
	RPL_TOPIC = 332,			//<channel> :<topic>	Visual
	RPL_INVITEING = 341,		//<nick> <channel>	Invito riuscito.
	RPL_NAMREPLY = 353,         // "= <channel> :<nick1> <nick2> ..."
	RPL_ENDOFNAMES = 366,       // "<channel> :End of /NAMES list"

	ERR_NOSUCHNICK = 401,		//<nick> :No such nick/channel	Il nickname o canale specificato non esiste.
	ERR_NOSUCHCHANNEL = 403,	//<channel> :No such channel	Canale non esistente.
	ERR_CANNOTSENDTOCHAN = 404,	//<channel> :Cannot send to channel	Il client non può inviare messaggi al canale.
	ERR_TOOMANYCHANNELS = 405,	//<channel> :You have joined too many channels	Limite di canali raggiunto per un utente.
	ERR_NOORIGIN = 409,        // :No origin specified	Comando senza origine.
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
	ERR_ALREADYREGISTRED = 462, // ":You may not reregister"	//Il client ha già completato la registrazione.
	ERR_PASSWDMISMATCH = 464,	//Password incorrect	Password errata.
	ERR_KEYSET = 467,           // "<channel> :Channel key already set"
	ERR_CHANNELISFULL = 471,	//Cannot join channel (+l)	Canale pieno.
	ERR_UNKNOWNMODE = 472,		//"<char> :is unknown mode char to me"	Modalità sconosciuta.
	ERR_INVITEONLYCHAN = 473,	//Cannot join channel (+i)	Canale solo su invito.
	ERR_BANNEDFROMCHAN = 474,	//Cannot join channel (+b)	L’utente è bannato dal canale.
	ERR_BADCHANNELKEY = 475,	//Cannot join channel (+k)	Password del canale errata.
	ERR_NOPRIVILEGES = 481,		//Permission Denied- You're not an IRC operator	Comando riservato agli operatori.
	ERR_CHANOPRIVSNEEDED = 482,	//You're not channel operator	L’utente non è operatore del canale.
	ERR_CANTKILLSERVER = 483,	//You can't kill a server!	Comando KILL verso un server.
	ERR_UMODEUNKNOWNFLAG = 501, //Unknown MODE flag	Modalità utente sconosciuta.
}	t_status;


class ReplyHandler
{
	private:
			const static std::string _prefix;
	public:
			static void	errorHandler(t_status, const User&, const std::string&, const std::string&);
			static void	replyHandler(t_status, const User&, const Channel*, const User*);
};

#endif
