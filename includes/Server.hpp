#ifndef SERVER_HPP
# define SERVER_HPP

//------------------EXTERN INCLUDES

# include "config.hpp"
# include <csignal>
# include <poll.h>
# include <arpa/inet.h>
# include <string>
# include <vector>
# include <map>

// # define PING_INTERVAL 60 // Server invia un PING ogni 60 secondi
// # define PONG_TIMEOUT 120 // Timeout per risposta del client con PONG

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
   
        void                    initSocket();
        void                    run();
        bool                    channelNameExists(const std::string &channelName);
        bool                    userNickEsists(const std::string &nickName);
        User*                   getUserByNick(const std::string &nickName);
        Channel*                getChannelByName(const std::string &channelName);
        std::vector<Channel*>   getChannelVector() const;
        std::vector<User*>      getUserVector() const;
        Channel&                createChannel(const std::string channelName);
        void                    sendMessage(std::string target, std::string message);
        std::string&            getPassword();
        void		            deleteChannel(Channel *toDelete);
		void                    disconnectUser(User* user);

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
    void                receiveNewMessage(size_t& iterator);
    // void                sendPing();
    // void                checkPong();
};

#endif