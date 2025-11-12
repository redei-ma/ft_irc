/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpirozzi <gpirozzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 14:55:56 by gpirozzi          #+#    #+#             */
/*   Updated: 2025/11/12 17:26:31 by gpirozzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include <cstring>
#include <unistd.h>
#include <iostream>
# include <sys/socket.h>

/* ================CONSTRUCTORS================ */
User::User():
			_fd(-1), _userName(""),
			_nickName(""), _password(""),
				_isAuthenticated(false), _hasNickName(false),
					_hasUserName(false), _hasPassword(false)
{}

User::User(int fd) :  _fd(-1), _userName(""), _nickName(""), _password(""),
				_isAuthenticated(false), _hasNickName(false), _hasUserName(false), _hasPassword(false)
{
	_fd = fd;
}

User::User(const User &other) : _userName(other.getUserName()), _nickName(other.getNickName()), _password(other.getPassword()),
									_isAuthenticated(other.getIsAuthenticated()), _hasNickName(other.getHasNickName()), _hasUserName(other.getHasUserName()),
									_hasPassword(other.getHasPassword())
{}

/* ================OPERATOR================ */
User&	User::operator=(const User& other)
{
	if (this != &other)
	{
		_userName = other.getUserName();
		_nickName = other.getNickName();
		_password = other.getPassword();

		_hasUserName = other.getHasUserName();
		_hasNickName = other.getHasNickName();
		_hasPassword = other.getHasPassword();
	}
	return (*this);
}

/* ================METHODS================ */
void	User::updateStrBuffer(char *buffer, size_t size)
{
	_bufferStr.append(buffer, size);
}

void	User::resetBuffer()
{
	_bufferStr = "";
}

void	User::updateAuthenticationStatus()
{
	_isAuthenticated = (getHasUserName() && getHasNickName() && getHasPassword());
}

void	User::sendMessage(std::string message) const
{
	try
	{
		std::string result(message + "\r\n");
		if (send(_fd, (void *)result.c_str(), result.size(), 0) < 0)
			throw std::runtime_error("Error in sending the message");
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

void	User::closeConnection()
{
	if (_fd > 0)
	{
		close(_fd);
		_fd = -1;
	}
}

/* ================SETTERS================ */
void	User::setUserName(std::string userName)
{
	_userName = userName;
	_hasUserName = true;
	updateAuthenticationStatus();
}

void	User::setNickName(std::string nickName)
{
	_nickName = nickName;
	_hasNickName = true;
	updateAuthenticationStatus();
}

void	User::setPassword(std::string password)
{
	_password = password;
	_hasPassword = true;
	updateAuthenticationStatus();
}

void	User::setLastPongTime()
{
	_lastPongTime = time(NULL);
}

/* ================GETTERS================ */
const std::string	User::getUserName() const { return (_userName); }

const std::string	User::getNickName() const { return (_nickName); }

const std::string	User::getPassword() const { return (_password); }

const bool	User::getIsAuthenticated() const { return (_isAuthenticated); }

const bool	User::getHasUserName() const { return (_hasUserName); }

const bool	User::getHasNickName() const { return (_hasNickName); }

const bool	User::getHasPassword() const { return (_hasPassword); }

const int	User::getUserFd() const { return (_fd); }

const time_t	User::getLastPongTime() const { return (_lastPongTime); }

/* const char*	User::getRcvBuffer() const { return (_bufferRcv); } */

const std::string	User::getStrBuffer() const { return (_bufferStr); };

/* ================DESTRUCTOR================ */
User::~User()
{
	closeConnection();
}