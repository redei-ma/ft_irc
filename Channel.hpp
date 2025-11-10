#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>

class User;

class Channel
{
	public:
		std::string			_name;
		bool				_isPrivate;
		std::string			_topic;
		bool				_isTopicPrivate;
		bool				_isInviteOnly;
		bool				_usersLimit;
		std::vector<User*>	_users;

		Channel();
		~Channel();
	
	private:	
};

#endif
