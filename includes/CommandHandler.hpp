#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include <vector>
# include <string>
# include "ReplyHandler.hpp"
# include <map>
# define DECLERE_COMMAND(name) void name##Command(User*, std::vector<std::string>&)

#include "Server.hpp"

typedef enum	s_command
{
	PASS,
	NICK,
	USER,
	JOIN,
	PRVMSG,
	KICK,
	INVITE,
	TOPIC,
	MODE,
	NOT_FOUND,
}	t_command;

class	CommandHandler
{
	private:
			Server&		_server;
			std::map<t_command, void (CommandHandler::*)(User*, std::vector<std::string>&)> commandMap;

			void		initCommand();

			DECLERE_COMMAND(join);
			DECLERE_COMMAND(pass);
			DECLERE_COMMAND(nick);
			DECLERE_COMMAND(user);
			DECLERE_COMMAND(prvmsg);
			DECLERE_COMMAND(kick);
			DECLERE_COMMAND(invite);
			DECLERE_COMMAND(topic);
			DECLERE_COMMAND(mode);
			DECLERE_COMMAND(ping);

			t_command	recognizeCommand(std::string);
			void		parseCommandArgs(std::string);
			public:
			CommandHandler(Server&);
			CommandHandler(const CommandHandler&);
			~CommandHandler();

			CommandHandler& operator=(const CommandHandler&);
			void		execCommand(User*, std::string);

};

#endif