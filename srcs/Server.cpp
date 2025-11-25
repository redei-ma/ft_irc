#include "config.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "CommandHandler.hpp"

#include <cstring>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <algorithm>
#include <cstdlib>
// #include <sys/socket.h>

// #include <stdlib.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <netinet/in.h>

/*-------------------------------------- SIGNAL --------------------------------------*/

volatile sig_atomic_t _serverRunning = 1;                                                  // Server is normally running

void	handle_sigint(int)
{
	_serverRunning = 0;																	   // If a CTRL+C(SIGINT) arrives, the core loop is interrupted and close() and delete() are called 
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
	this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0); 								   // AF_INET handles 127.0.0.1(Localhost) which uses IPv4
	if (this->_serverSocket == -1)
		throw(std::runtime_error("Error: something went wrong in the call to socket()."));
	int reuse = 1;
	if (setsockopt(this->_serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		close(_serverSocket);
		throw(std::runtime_error("Error: something went wrong in the call to setsockopt()."));
	}
	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) < 0)                                     // O_NONBLOCK make the server NON-BLOCKING. 
	{
		close(_serverSocket);
		throw(std::runtime_error("Error: something went wrong in the call to fcntl()."));
	}
	this->sinInit();																	   // Initializes the sin struct with 0 and then fills it.
	if (!this->bindSocket())															   // bind() on socket, associate the port with the socket server fd.
	{
		close(_serverSocket);
		throw(std::runtime_error("Error: something went wrong in the call to bind()."));
	}
	if (!this->putInListen())                                                              // Call listen() which make the server in listen-mode (waiting for a new connection request)
	{
		close(_serverSocket);
		throw(std::runtime_error("Error: something went wrong int he call to listen()."));
	}
}

void    Server::sinInit()
{
	memset(&_serverSin, 0, sizeof(_serverSin));
	_serverSin.sin_port = htons(_port);
	_serverSin.sin_addr.s_addr = INADDR_ANY;
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
	if (listen(this->_serverSocket, 1024) < 0)
		return false;
	return true;
}

bool Server::acceptNewConnection()
{
    sockaddr_in clientAddr;
    socklen_t addrlen = sizeof(clientAddr);

    int tmpFd = accept(_serverSocket, (struct sockaddr*)&clientAddr, &addrlen);
    if (tmpFd == -1)
    {
        perror("accept");
        return false;
	}
	if (_userNbr >= MAX_USER)
    {
        const char *errorMsg = "Error: max capacity reached. Connection closed.";
        send(tmpFd, errorMsg, strlen(errorMsg), MSG_NOSIGNAL);
        close(tmpFd);
        return false;
    }
    if (fcntl(tmpFd, F_SETFL, O_NONBLOCK) < 0)
	{
		perror("fcntl");
		close(tmpFd);
		return false;
	}
    struct pollfd tmpPoll;
    tmpPoll.fd = tmpFd;
    tmpPoll.events = POLLIN;
    tmpPoll.revents = 0;

	std::cout << "Nuovo utente connesso" << std::endl;
	_pollVector.push_back(tmpPoll);
	User *tmpUser = new User(tmpFd, clientAddr);
	_fdUserMap[tmpFd] = tmpUser;
	_userNbr++;

	return true;
}

void	Server::receiveNewMessage(size_t& iterator)
{
	std::cout << "Receiving new message..." << std::endl;
	char buffer[BUFFER_SIZE + 1]; 														      // Max 512 characters for buffer, + 1 for terminal.
	ssize_t size = recv(CLIENT, buffer, BUFFER_SIZE, 0);
	if (size <= 0)                                                                // A client disconnected.
	{
		_command->execCommand(_fdUserMap[CLIENT], "QUIT\r\n");
		iterator--;
		return ;
	}
	buffer[size] = '\0';
	_fdUserMap[CLIENT]->updateStrBuffer(buffer, std::strlen(buffer));
	if (_fdUserMap[CLIENT]->getStrBuffer().find("\r\n") != std::string::npos) // Reached the end of message. (delimited by : \r\n) 
	{
		std::string commandToExec = _fdUserMap[CLIENT]->getStrBuffer();
		_fdUserMap[CLIENT]->resetBuffer();
		std::cout << "Received message from user " << _fdUserMap[CLIENT]->getNickName() << ": " << commandToExec << std::endl;
		_command->execCommand(_fdUserMap[CLIENT], commandToExec);
		if (commandToExec.substr(0, 4) == "QUIT")
			iterator--;
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
	for (std::map<int, User*>::iterator it = _fdUserMap.begin(); it != _fdUserMap.end(); ++it)
	{
		if (it->second->getNickName() == nickName)
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
	for (std::map<int, User*>::iterator it = _fdUserMap.begin(); it != _fdUserMap.end(); ++it)
	{
		if (it->second->getNickName() == nickName)
			return it->second;
	}
	return NULL;
}

std::vector<User*>	Server::getUserVector() const
{
	std::vector<User*>	userVector;
	std::map<int, User*>::const_iterator	it;

	for (it = _fdUserMap.begin(); it != _fdUserMap.end(); ++it)
	{
		userVector.push_back((it->second));
	}
	return (userVector);
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

void	Server::deleteChannel(Channel *toDelete)
{
	std::vector<Channel *>::iterator it = std::find(this->_channelVector.begin(), this->_channelVector.end(), toDelete);
	if (it != this->_channelVector.end())
	{
		delete(*it);
		this->_channelVector.erase(it);
	}
	return;
}

void	Server::disconnectUser(User* user)
{
	int userFd = user->getUserFd();
	std::map<int, User*>::iterator it = _fdUserMap.find(userFd);
	if (it == _fdUserMap.end())
		return;
	delete (_fdUserMap[userFd]);
	for (std::vector<pollfd>::iterator pit = _pollVector.begin(); pit != _pollVector.end(); ++pit)
	{
		if (pit->fd == userFd)
		{
			_pollVector.erase(pit);
			break;
		}
	}
	_fdUserMap.erase(it);
	_userNbr--;
	return;
}

/*------------------------------------------------CORE LOOP----------------------------------------------------------*/

void	Server::run()
{
	std::cout << "Server is running on port " << this->_port << std::endl;

	_pollVector[0].fd = this->_serverSocket;
	_pollVector[0].events = POLLIN;
	_pollVector[0].revents = 0;
	while (_serverRunning)
	{
		int ret = poll(_pollVector.data(), _pollVector.size(), -1); // poll()
		if (ret == -1)
			break;
		for (size_t i = 0; i < _pollVector.size(); i++)
		{
			if (_pollVector[i].revents & POLLIN)
			{
				if (_pollVector[i].fd == this->_serverSocket)       // If poll receive something on the server socket, it'll be a new connection request.
				{
					if (!acceptNewConnection())                     // Accept new connection, listened by listen().
						continue ;
				}
				else
				{
					receiveNewMessage(i);                           // receive the message by recv() and handles the buffers.
				}											    // else, it only can be a message.
			}
		}
	}
}
