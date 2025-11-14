#ifndef SERVER_HPP
# define SERVER_HPP

//------------------EXTERN INCLUDES
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/types.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <csignal>
# include <stdlib.h>
# include <poll.h>
# include <vector>
# include <map>

//-----------------INTERN INCLUDES
# include "User.hpp"
# include "Channel.hpp"

# define MAX_USR_NBR 5 // Max user capacity.

//---------------------------RECOGNIZE OTHER CLASSES-----------------------------            

class Channel;
class User;
class CommandHandler;

//--------------------------------SIGNAL HANDLING--------------------------------

extern volatile sig_atomic_t _serverRunning;
void    handle_sigint(int);

//-------------------------------------------------------------------------------

class Server
{
    public :
    /*--------------------     O.C.F.     --------------------*/
                            Server();
                            Server(int _portNbr, std::string _pass);
                            Server(const Server &other);
        Server&             operator=(const Server &other);
                            ~Server();
    
    /*-------------------- Public Methods --------------------*/
   
        void                initSocket();
        void                run();
        bool                channelNameExists(const std::string &channelName);
        bool                userNickEsists(const std::string &nickName);
        Channel*            getChannelName(const std::string &channelName);
        void                sendMessage(std::string target, std::string message);
        std::string&        getPassword();

    private :
    /*-------------------- Core Variables --------------------*/

        int                     _port;
        int                     _serverSocket;
        int                     _userNbr;
        struct sockaddr_in      _serverSin;
        std::string             _password;
        std::vector<pollfd>     _pollVector;
        std::vector<Channel*>   _channelVector;
        std::map<int, User*>    _fdUserMap;
        CommandHandler          *_command;

    /*-------------------- Private Methods --------------------*/
    void                sinInit();
    bool                bindSocket();
    bool                putInListen();
    bool                acceptNewConnection();
    void                receiveNewMessage(int iterator);
};

#endif