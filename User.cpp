/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpirozzi <gpirozzi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 14:55:56 by gpirozzi          #+#    #+#             */
/*   Updated: 2025/11/13 13:32:06 by gpirozzi         ###   ########.fr       */
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
			_bufferStr(""), _isAuthenticated(false),
			_hasNickName(false), _hasUserName(false),
			_hasPassword(false), _lastPongTime(0)
{}

User::User(int fd) :
				_fd(fd), _userName(""),
				_nickName(""), _password(""),
				_bufferStr(""), _isAuthenticated(false),
				_hasNickName(false), _hasUserName(false),
				_hasPassword(false), _lastPongTime(0)
{}

User::User(const User &other) :
							_fd(getUserFd()), _userName(other.getUserName()),
							_nickName(other.getNickName()), _password(other.getPassword()),
							_bufferStr(other.getStrBuffer()), _isAuthenticated(other.getIsAuthenticated()),
							_hasNickName(other.getHasNickName()), _hasUserName(other.getHasUserName()),
							_hasPassword(other.getHasPassword()), _lastPongTime(0)
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
std::string	User::getUserName() const { return (_userName); }

std::string	User::getNickName() const { return (_nickName); }

std::string	User::getPassword() const { return (_password); }

bool	User::getIsAuthenticated() const { return (_isAuthenticated); }

bool	User::getHasUserName() const { return (_hasUserName); }

bool	User::getHasNickName() const { return (_hasNickName); }

bool	User::getHasPassword() const { return (_hasPassword); }

int	User::getUserFd() const { return (_fd); }

time_t	User::getLastPongTime() const { return (_lastPongTime); }

std::string	User::getStrBuffer() const { return (_bufferStr); };

/* ================DESTRUCTOR================ */
User::~User()
{
	closeConnection();
}