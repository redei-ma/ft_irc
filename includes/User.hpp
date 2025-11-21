#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <time.h>
#include <netinet/in.h>
# include <vector>

class Channel;

class User
{
	private:
				const int	_fd;
				
				std::string	_userName;
				std::string	_nickName;
				std::string	_password;
				std::string	_bufferStr;
				std::string	_realName;
				sockaddr_in	_hostName;

				std::vector<Channel*>	_channelVector;

				bool	_isAuthenticated;
				bool	_hasNickName;
				bool	_hasUserName;
				bool	_hasPassword;

				time_t	_lastPongTime;

				User();
	public:
			User(const int&, const sockaddr_in&);
			User(const User&);
			User& operator=(const User&);
			bool operator!=(const User&);

			void	setUserName(const std::string&);
			void	setNickName(const std::string&);
			void	setPassword(const std::string&);
			void	setLastPongTime();
			void	setRealName(const std::string&);

			bool	getIsAuthenticated() const;
			bool	getHasUserName() const;
			bool	getHasNickName() const;
			bool	getHasPassword() const;

			time_t	getLastPongTime() const;
			
			std::vector<Channel*>&	getChannelVector();

			int getUserFd() const;

			const std::string	getUserName() const;
			const std::string	getNickName() const;
			const std::string	getPassword() const;
			const std::string	getStrBuffer() const;
			const std::string	getRealName() const;
			const std::string	getHostNameAsString() const;
			const std::string	getPrefix() const;

			sockaddr_in	getHostName() const;

			void	joinChannel(Channel*);
			void	exitChannel(Channel*);
			void	updateStrBuffer(char*, size_t);
			void	updateAuthenticationStatus();
			void	sendMessage(const std::string&) const;
			void	closeConnection();
			void	resetBuffer();

			~User();
};

#endif