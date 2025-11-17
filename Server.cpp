#include "Server.hpp"
#include "CommandHandler.hpp"

# include <cstring>
# include <stdio.h>
# include <iostream>
# include <fcntl.h>

/*-------------------------------------- SIGNAL --------------------------------------*/

volatile sig_atomic_t _serverRunning = 1;

void	handle_sigint(int)
{
	_serverRunning = 0;
	std::cerr << "\nClosing server al cleaning datas." << std::endl;
}

/*-------------------------------------- O.C.F. --------------------------------------*/

Server::Server() : _port(0),  _userNbr(0), _serverSin(), _password(""), _pollVector(1)
{
	this->_command = new CommandHandler(*this);
}

Server::Server(const Server &other) : _port(other._port),  _userNbr(0), _serverSin(other._serverSin), _password(other._password), _pollVector(1)
{
	this->_command = new CommandHandler(*this);
}

Server::Server(int _portNbr, std::string _pass) : _port(_portNbr),  _userNbr(0), _serverSin(), _password(_pass), _pollVector(1)
{
	this->_command = new CommandHandler(*this);
	initSocket();
}

Server&	Server::operator=(const Server &other)
{
	if (this != &other)
	{
		this->_port = other._port;
		this->_userNbr = other._userNbr;
		this->_serverSin = other._serverSin;
		this->_password = other._password;
		this->_pollVector = other._pollVector;
	}
	return *this;
}

Server::~Server()
{
	delete(_command);

	for (size_t i = 0; i < _fdUserMap.size(); i++)
		delete(_fdUserMap[i]);

	for (size_t i = 0; i < _channelVector.size(); i++)
		delete(_channelVector[i]);

	close(this->_serverSocket);
}

/*-------------------------------------- Methods --------------------------------------*/

void    Server::initSocket()
{
	this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET gestisce 127.0.0.1 che utilizza IPv4
	if (this->_serverSocket == -1)
		throw(std::runtime_error("Error: something went wrong in the call to socket()."));

	this->sinInit();

	if (!this->bindSocket())
		throw(std::runtime_error("Error: something went wrong in the call to bind()."));
	if (!this->putInListen())
		throw(std::runtime_error("Error: something went wrong int he call to listen()."));

	// non viene mai chiuso il socket del server se lanciata l'eccezione Renato
}

void    Server::sinInit()
{
	memset(&_serverSin, 0, sizeof(_serverSin));
	_serverSin.sin_port = htons(_port);
	_serverSin.sin_addr.s_addr = inet_addr("127.0.0.1");
	_serverSin.sin_family = AF_INET;
}

bool	Server::bindSocket()
{
	if (bind(this->_serverSocket, (sockaddr *)&this->_serverSin, sizeof(_serverSin)) < 0)
		return false;
	return true;
}

bool	Server::putInListen()
{
	int reuse = 1;
	if (setsockopt(this->_serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
		return false;
	if (listen(this->_serverSocket, 1024) < 0)
		return false;
	return true;
}

bool	Server::acceptNewConnection()
{
	int tmpFd = accept(_serverSocket, NULL, NULL);
	if (tmpFd == -1)
	{
		std::cerr << "Error: something went wrong in the call to accept()." << std::endl;
	}
	fcntl(tmpFd, F_SETFL, O_NONBLOCK);
	struct pollfd tmpPoll;
	tmpPoll.fd = tmpFd;
	tmpPoll.events = POLLIN;
	tmpPoll.revents = 0;
	if (_userNbr >= MAX_USR_NBR)
	{
		const char *errorMsg = "Error: max capacity of user has been reached. Connection will be interrupted now.";
		send(tmpFd, errorMsg, 81, 0);
		close(tmpFd);
		return false ;
	}
	else
	{
		std::cout << "Nuovo utente connesso" << std::endl;
		_pollVector.push_back(tmpPoll);
		User *tmpUser = new User(tmpFd);
		this->_fdUserMap[tmpFd] = tmpUser;
		_userNbr++;
	}
	return true;
}

void	Server::receiveNewMessage(int iterator)
{
	char buffer[513];
	ssize_t size = recv(_pollVector[iterator].fd, buffer, sizeof(buffer) - 1, 0);
	if (size <= 0)
	{
		std::cout << "connesione chiusa" << std::endl;
		std::map<int, User*>::iterator it = _fdUserMap.find(_pollVector[iterator].fd);
		delete (_fdUserMap[_pollVector[iterator].fd]);
		_pollVector.erase(_pollVector.begin() + iterator);
		_fdUserMap.erase(it);
		_userNbr--;
		iterator--;
		return ;
	}
	buffer[size] = '\0';
	_fdUserMap[_pollVector[iterator].fd]->updateStrBuffer(buffer, std::strlen(buffer));
	if (_fdUserMap[_pollVector[iterator].fd]->getStrBuffer().find("\r\n") != std::string::npos)
	{
		_command->execCommand(_fdUserMap[_pollVector[iterator].fd], _fdUserMap[_pollVector[iterator].fd]->getStrBuffer());
		_fdUserMap[_pollVector[iterator].fd]->resetBuffer();
	}
	return ;
}

bool            Server::channelNameExists(const std::string &channelName)
{
	for (size_t i = 0; i < _channelVector.size(); i++)
	{
		if (_channelVector[i]->getName() == channelName)
			return true;
	}
	return false;
}

bool            Server::userNickEsists(const std::string &nickName)
{
	//parte da 1 perche' pollvector[0]=>server socket
	for (size_t i = 1; i < _fdUserMap.size(); i++)
	{
		if (_fdUserMap[_pollVector[i].fd]->getNickName() == nickName)
			return true;
	}
	return false;
}

Channel*       Server::getChannelByName(const std::string &channelName)
{
	for (size_t i = 0; i < _channelVector.size(); i++)
	{
		if (_channelVector[i]->getName() == channelName)
			return _channelVector[i];
	}
	return NULL;
}

User*               Server::getUserByNick(const std::string &nickName)
{
	for (size_t i = 0; i < _fdUserMap.size(); i++)
	{
		if (_fdUserMap[_pollVector[i].fd]->getUserName() == nickName)
			return _fdUserMap[_pollVector[i].fd];
	}
	return NULL;
}

Channel&	Server::createChannel(const std::string channelName)
{
	Channel	*newChannel = new Channel(channelName);
	_channelVector.push_back(newChannel);
	return (*newChannel);
}

std::vector<Channel*>   Server::getChannelVector() const
{
	return (_channelVector);
}

std::string&        Server::getPassword()
{
	return this->_password;
}

/*------------------------------------------------CORE LOOP----------------------------------------------------------*/

void	Server::run()
{
	std::cout << "Server running" << std::endl;

	_pollVector[0].fd = this->_serverSocket;
	_pollVector[0].events = POLLIN;
	_pollVector[0].revents = 0;
	while (_serverRunning)
	{
		int ret = poll(_pollVector.data(), _pollVector.size(), -1); //Non sono convinto (Renato non e' convinto e Giovanni e' un po scemo)
		if (ret == -1)
			break;
		fcntl(_serverSocket, F_SETFL, O_NONBLOCK);
		for (size_t i = 0; i < _pollVector.size(); i++)
		{
			if (_pollVector[i].revents & POLLIN)
			{
				if (_pollVector[i].fd == this->_serverSocket)
				{
					if (!acceptNewConnection())
						continue ;
				}
				else
					receiveNewMessage(i);
			}
		}
	}
}
