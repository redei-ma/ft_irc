#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <poll.h>
#include <vector>
#include <map>

class Channel;
class User;

class server
{
    public :
    /*-------------------- OCF --------------------*/ 
                            server();
                            server(int _portNbr, std::string _pass);
                            server(const server &other);
        server&             operator=(const server &other);
                            ~server();
    
    /*-------------------- Public Methods --------------------*/
   
        void                initSocket();
        void                run();

    private :
    /*-------------------- Core Variables --------------------*/

        int                     _port;
        int                     _serverSocket;
        struct sockaddr_in      _serverSin;
        std::string             _password;
        std::vector<pollfd>     _pollVector;
        std::vector<Channel>    _channelVector;
        std::map<int, User>     _fdUserMap;

    /*-------------------- Private Methods --------------------*/

        void                sinInit();
        bool                bindSocket();
        bool                putInListen();

};

#endif