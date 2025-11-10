/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpirozzi <gpirozzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 14:55:58 by gpirozzi          #+#    #+#             */
/*   Updated: 2025/11/10 18:57:57 by gpirozzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include <iostream>
# include <string>
# include <sys/socket.h>

class User
{
	private:
				int	_fd;

				bool	_isAuthenticated;
				bool	_hasUserName;
				bool	_hasNickName;
				bool	_hasPassword;

				std::string	_userName;
				std::string	_nickName;
				std::string	_password;

				time_t	_lastPongTime;

	public:
			User();
			User(int fd);
			User(const User& other);
			User& operator=(const User& other);

			void	setUserName(std::string userName);
			void	setNickName(std::string nickName);
			void	setPassword(std::string password);
			void	setLastPongTime();

			const bool	getIsAuthenticated() const;
			const bool	getHasUserName() const;
			const bool	getHasNickName() const;
			const bool	getHasPassword() const;
			
			const int getUserFd() const;

			const time_t	getLastPongTime() const;

			const std::string	getUserName() const;
			const std::string	getNickName() const;
			const std::string	getPassword() const;


			void	updateAuthenticationStatus();
			void	sendMessage(std::string message) const;
			void	closeConnection();

			~User();
};

#endif