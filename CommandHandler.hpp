/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpirozzi <gpirozzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 11:32:48 by gpirozzi          #+#    #+#             */
/*   Updated: 2025/11/11 16:41:25 by gpirozzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include <vector>
# include <string>
# include <map>

# define DECLERE_COMMAND(name) void name##Command(std::vector<std::string>)

class Server;

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
			void		initCommand();
			Server&		server;
			DECLERE_COMMAND(join);
			DECLERE_COMMAND(pass);
			DECLERE_COMMAND(nick);
			DECLERE_COMMAND(user);
			DECLERE_COMMAND(msg);
			DECLERE_COMMAND(kick);
			DECLERE_COMMAND(invite);
			DECLERE_COMMAND(topic);
			DECLERE_COMMAND(mode);
			DECLERE_COMMAND(ping);
			t_command	reconizeCommand(std::string command);
			std::map<t_command, void (CommandHandler::*)(std::vector<std::string>)> commandMap;
			void		parseCommandArgs(std::string args);
	public:
			CommandHandler(Server& server);
			CommandHandler(const CommandHandler& other);
			~CommandHandler();

			CommandHandler& operator=(const CommandHandler& other);
			void		execCommand(std::string commandAndArgs);
};

#endif