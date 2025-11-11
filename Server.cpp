#include "Server.hpp"
#include "User.hpp"

/*------------------- OCF -------------------*/

Server::Server() : _password(""), _port(0), _serverSin() {}

Server::Server(const Server &other) : _password(other._password), _port(other._port), _serverSin(other._serverSin), _channelVector(0), _fdUserMap(0, 0){}

Server::Server(int _portNbr, std::string _pass) : _pollVector(1), _port(_portNbr), _password(_pass), _channelVector(0), _fdUserMap(0, 0)
{
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

}

Server::~Server()
{
        close(this->_serverSocket);
}

/*------------------- Methods -------------------*/

void    Server::sinInit()
{
    memset(&_serverSin, 0, sizeof(_serverSin));
    _serverSin.sin_port = htons(5555);
    _serverSin.sin_addr.s_addr = inet_addr("127.0.0.1");
    _serverSin.sin_family = AF_INET;
}/*--- altri eventi ---*/

bool    Server::bindSocket()
{
    return bind(this->_serverSocket, (sockaddr *)&this->_serverSin, sizeof(_serverSin));
}/*--- altri eventi ---*/

bool    Server::putInListen()
{
    return listen(this->_serverSocket, 1024);
}

void    Server::run()
{
    _pollVector[0].fd = this->_serverSocket;
    _pollVector[0].events = POLLIN;
    _pollVector[0].revents = 0;
    while (1)
    {
        int ret = poll(_pollVector.data(), _pollVector.size(), -1); //non sono covinto
        if (ret == -1)
            throw(std::runtime_error("Error: something went wrong in the call to poll()."));
        for (size_t i = 0; i < _pollVector.size(); i++)
        {
            if (_pollVector[i].revents & POLLIN)
            {
                if (_pollVector[i].fd == this->_serverSocket)
                {
                    int tmpFd = accept(_serverSocket, NULL, NULL);
                    if (tmpFd == -1)
                        throw (std::runtime_error("Error: something went wrong in the call to accept()."));
                    struct pollfd tmpPoll;
                    tmpPoll.fd = tmpFd;
                    tmpPoll.events = POLLIN;
                    tmpPoll.revents = 0;
                    _pollVector.push_back(tmpPoll);
                    User tmpUser(tmpFd);
                    this->_fdUserMap.insert(std::make_pair(tmpFd, tmpUser));
                }
                else
                {
                    char buffer[513];
                    size_t size = recv(_pollVector[i].fd, buffer, 512, 0);
                    buffer[size] = '\0';
                    /* if ()
                    {
                        
                    }
                    else if ()
                    {

                    }  
                    else
                    {
                    } */
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