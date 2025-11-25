#include "User.hpp"
#include "Channel.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <algorithm>
#include <arpa/inet.h>

/* ================CONSTRUCTORS================ */
User::User():
			_fd(-1), _userName(""),
			_nickName(""), _password(""),
			_bufferStr(""), _realName(""),
			_isAuthenticated(false), _hasNickName(false), _hasUserName(false),
			_hasPassword(false), _lastPongTime(0)
{
	memset(&_hostName, 0, sizeof(_hostName));
}

User::User(const int& fd, const sockaddr_in& hostName) :
			_fd(fd), _userName(""),
			_nickName(""), _password(""),
			_bufferStr(""), _realName(""),  _hostName(hostName),
			_isAuthenticated(false), _hasNickName(false), _hasUserName(false),
			_hasPassword(false), _lastPongTime(0)
{}

User::User(const User& other) :
			_fd(getUserFd()), _userName(other.getUserName()),
			_nickName(other.getNickName()), _password(other.getPassword()),
			_bufferStr(other.getStrBuffer()), _realName(other.getRealName()), _hostName(other.getHostName()),
			_isAuthenticated(other.getIsAuthenticated()), _hasNickName(other.getHasNickName()),
			_hasUserName(other.getHasUserName()), _hasPassword(other.getHasPassword()), _lastPongTime(other.getLastPongTime())
{}

/* ================OPERATOR================ */
User&	User::operator=(const User& other)
{
	if (this != &other)
	{
		_userName = other.getUserName();
		_nickName = other.getNickName();
		_password = other.getPassword();
		_bufferStr = other.getStrBuffer();
		_realName = other.getRealName();
		_hostName = other.getHostName();

		_hasUserName = other.getHasUserName();
		_hasNickName = other.getHasNickName();
		_hasPassword = other.getHasPassword();
	}
	return (*this);
}

bool User::operator!=(const User& other)
{
	return (_fd != other.getUserFd());
}

/* ================METHODS================ */
void	User::updateStrBuffer(char *buffer, size_t size)
{
	_bufferStr.append(buffer, size);
}

void	User::resetBuffer()
{
	_bufferStr.clear();
}

void	User::updateAuthenticationStatus()
{
	_isAuthenticated = (getHasUserName() && getHasNickName() && getHasPassword());
}

void	User::sendMessage(const std::string& message) const
{
	try
	{
		std::string result(message + "\r\n");
		if (send(_fd, result.c_str(), result.size(), MSG_NOSIGNAL) <= 0)
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
		close(_fd);
}

void	User::joinChannel(Channel* channel)
{
	if (channel != NULL)
		_channelVector.push_back(channel);
}

void	User::exitChannel(Channel* channel)
{
	std::vector<Channel*>::iterator it = std::find(_channelVector.begin(), _channelVector.end(), channel);

	if (it != _channelVector.end())
		_channelVector.erase(it);
}

/* ================SETTERS================ */
void	User::setUserName(const std::string& userName)
{
	_userName = userName;
	_hasUserName = true;
	updateAuthenticationStatus();
}

void	User::setNickName(const std::string& nickName)
{
	_nickName = nickName;
	_hasNickName = true;
	updateAuthenticationStatus();
}

void	User::setPassword(const std::string& password)
{
	_password = password;
	_hasPassword = true;
	updateAuthenticationStatus();
}

void	User::setLastPongTime()
{
	_lastPongTime = time(NULL);
}

void	User::setRealName(const std::string& realName) { _realName = realName;}

/* ================GETTERS================ */
const std::string	User::getUserName() const { return (_userName); }

const std::string	User::getNickName() const { return (_nickName); }

const std::string	User::getPassword() const { return (_password); }

const std::string	User::getRealName() const { return (_realName); }

sockaddr_in	User::getHostName() const { return (_hostName); }

const std::string	User::getHostNameAsString() const { return (inet_ntoa(_hostName.sin_addr)); }

bool	User::getIsAuthenticated() const { return (_isAuthenticated); }

bool	User::getHasUserName() const { return (_hasUserName); }

bool	User::getHasNickName() const { return (_hasNickName); }

bool	User::getHasPassword() const { return (_hasPassword); }

int	User::getUserFd() const { return (_fd); }

time_t	User::getLastPongTime() const { return (_lastPongTime); }

const std::string	User::getStrBuffer() const { return (_bufferStr); };

std::vector<Channel*>&	User::getChannelVector() { return (_channelVector);}

const std::string	User::getPrefix() const { return (":" + _nickName + "!" + _userName + "@" + getHostNameAsString()); }

/* ================DESTRUCTOR================ */
User::~User()
{
	closeConnection();
}