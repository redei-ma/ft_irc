#include "Server.hpp"
#include "CommandHandler.hpp"

# include <cstring>
# include <stdio.h>
# include <iostream>

/*-------------------------------------- SIGNAL --------------------------------------*/

volatile sig_atomic_t _serverRunning = 1;

void    handle_sigint(int)
{
    _serverRunning = 0;
    std::cerr << "\nClosing server al cleaning datas." << std::endl;
}

/*-------------------------------------- OCF --------------------------------------*/

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
    try
    {
        initSocket();
        run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

Server&     Server::operator=(const Server &other)
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
    delete(_fdUserMap[0]);
    for (size_t i = 1; i < _fdUserMap.size(); i++)
    {
        close(_pollVector[i].fd);
        delete(_fdUserMap[i]);
    }
    close(this->_serverSocket);
}

/*-------------------------------------- Methods --------------------------------------*/

void    Server::sinInit()
{
    memset(&_serverSin, 0, sizeof(_serverSin));
    _serverSin.sin_port = htons(_port);
    _serverSin.sin_addr.s_addr = inet_addr("127.0.0.1");
    _serverSin.sin_family = AF_INET;
}

bool    Server::bindSocket()
{
    if (bind(this->_serverSocket, (sockaddr *)&this->_serverSin, sizeof(_serverSin)) < 0)
        return false;
    return true;
}

bool    Server::putInListen()
{
    int reuse = 1;
    setsockopt(this->_serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (listen(this->_serverSocket, 1024) < 0)
        return false;
    return true;
}

bool    Server::acceptNewConnection()
{
    int tmpFd = accept(_serverSocket, NULL, NULL);
    if (tmpFd == -1)
    {
        std::cerr << "Error: something went wrong in the call to accept()." << std::endl;
        
    }
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
        _pollVector.push_back(tmpPoll);
        User *tmpUser = new User(tmpFd);
        this->_fdUserMap[tmpFd] = tmpUser;
        _userNbr++;
    }
    return true;
}

void                Server::receiveNewMessage(int i)
{
    while (_serverRunning)
    {
        char buffer[513];
        size_t size = recv(_pollVector[i].fd, buffer, sizeof(buffer) - 1, 0);
        if (size == (size_t)0)
        {
            std::cout << "connesione chiusa" << std::endl, close(_pollVector[i].fd), _pollVector.erase(_pollVector.begin() + i);
            i--, _userNbr--;
            continue ;
        }
        else if (size == (size_t)-1)
        {
            std::cout << "Error: recv() returned -1." << std::endl, close(_pollVector[i].fd), _pollVector.erase(_pollVector.begin() + i);
            i--, _userNbr--;
            continue ;
        }
        buffer[size] = '\0';
        _fdUserMap[_pollVector[i].fd]->updateStrBuffer(buffer, std::strlen(buffer));
        if (_fdUserMap[_pollVector[i].fd]->getStrBuffer().find("\r\n") != std::string::npos)
        {
            _command->execCommand(_fdUserMap[_pollVector[i].fd], _fdUserMap[_pollVector[i].fd]->getStrBuffer());
            _fdUserMap[_pollVector[i].fd]->resetBuffer();
        }
        return ;
    }
}


void    Server::run()
{
    _pollVector[0].fd = this->_serverSocket;
    _pollVector[0].events = POLLIN;
    _pollVector[0].revents = 0;
    while (_serverRunning)
    {
        int ret = poll(_pollVector.data(), _pollVector.size(), -1); //non sono convinto (Renato non e' convinto)
        if (ret == -1)
            throw(std::runtime_error("Error: something went wrong in the call to poll()."));
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
                {
                    receiveNewMessage(i);
                }
            }
        }
    }
}

void    Server::initSocket()
{
    this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET gestisce 127.0.0.1 che utilizza IPv4
    if (this->_serverSocket == -1)
        throw(std::runtime_error("Error: something went wrong in the call to socket()."));
    this->sinInit();
    /* if (!this->sinInit())
        throw(std::runtime_error("Error: something went wrong during the sin_struct initialiation.")); */
    if (!this->bindSocket())
        throw(std::runtime_error("Error: something went wrong in the call to bind()."));
    if (!this->putInListen())
        throw(std::runtime_error("Error: something went wrong int he call to listen()."));
}