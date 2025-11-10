#include "server.hpp"

/*------------------- OCF -------------------*/

server::server() : _password(""), _port(0), _serverSin() {}

server::server(const server &other) : _password(other._password), _port(other._port), _serverSin(other._serverSin), _channelVector(0), _fdUserMap(0, 0){}

server::server(int _portNbr, std::string _pass) : _pollVector(1), _port(_portNbr), _password(_pass), _channelVector(0), _fdUserMap(0, 0)
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

server&     server::operator=(const server &other)
{

}

server::~server()
{
        close(this->_serverSocket);
}

/*------------------- Methods -------------------*/

void    server::sinInit()
{
    memset(&_serverSin, 0, sizeof(_serverSin));
    _serverSin.sin_port = htons(5555);
    _serverSin.sin_addr.s_addr = inet_addr("127.0.0.1");
    _serverSin.sin_family = AF_INET;
}

bool    server::bindSocket()
{
    return bind(this->_serverSocket, (sockaddr *)&this->_serverSin, sizeof(_serverSin));
}

bool    server::putInListen()
{
    return listen(this->_serverSocket, 1024);
}

void    server::run()
{
    _pollVector[0].fd = this->_serverSocket;
    _pollVector[0].events = POLLIN;
    _pollVector[0].revents = 0;
    while (1)
    {
        poll(_pollVector.data(), _pollVector.size(), -1);
        for (size_t i = 0; i < _pollVector.size(); i++)
        {
            if (_pollVector[i].revents & POLLIN)
            {
                if (_pollVector[i].fd == this->_serverSocket)
                {
                    int _tmpFd = accept(_serverSocket, NULL, NULL);
                    if (_tmpFd == -1)
                        throw (std::runtime_error("Error: something went wrong in the call to accept()."));
                    struct pollfd _tmpPoll;
                    _tmpPoll.fd = _tmpFd;
                    _tmpPoll.events = POLLIN;
                    _tmpPoll.revents = 0;
                    _pollVector.push_back(_tmpPoll);
                }
            }
            else
            {
                /*--- altri eventi ---*/
            }
            if (_pollVector[i].fd == -1)
                throw(std::runtime_error("Error: something went wrong in the call to poll()."));
        }
    }
}

void    server::initSocket()
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