#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <time.h>

class User
{
	private:
				int	_fd;
				
				std::string	_userName;
				std::string	_nickName;
				std::string	_password;
				std::string	_bufferStr;

				bool	_isAuthenticated;
				bool	_hasNickName;
				bool	_hasUserName;
				bool	_hasPassword;

				time_t	_lastPongTime;

				User();
	public:
			User(int fd);
			User(const User& other);
			User& operator=(const User& other);

			void	setUserName(std::string userName);
			void	setNickName(std::string nickName);
			void	setPassword(std::string password);
			void	setLastPongTime();

			bool	getIsAuthenticated() const;
			bool	getHasUserName() const;
			bool	getHasNickName() const;
			bool	getHasPassword() const;

			int getUserFd() const;

			time_t	getLastPongTime() const;

			std::string	getUserName() const;
			std::string	getNickName() const;
			std::string	getPassword() const;
			std::string	getStrBuffer() const;

			void		updateStrBuffer(char *buffer, size_t size);
			void		updateAuthenticationStatus();
			void		sendMessage(std::string message) const;
			void		closeConnection();
			void		resetBuffer();

			~User();
};

#endif