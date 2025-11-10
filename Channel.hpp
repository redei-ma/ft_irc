#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>

class User;

class Channel
{
	private:
		std::string			_name;
		bool				_hasPassword;
		std::string			_password;
		std::string			_topic;
		bool				_isTopicPrivate;
		bool				_isInviteOnly;
		size_t				_usersLimit;
		bool				_isFull;
		std::vector<User*>	_users;
		std::vector<User*>	_operators;
		std::vector<User*>	_invitedUsers;

		Channel();
		Channel(const Channel& other);
		Channel&	operator=(const Channel& other);

		bool 	isMember(User* user) const;
		bool 	isOperator(User* user) const;
		bool	isInvited(User* user) const;
		size_t	getUserCount() const;

	public:
		Channel(const std::string& name);
		~Channel();

		const std::string&			getName() const;

		bool						hasPassword() const;
		const std::string&			getPassword() const;
		void						setPassword(const std::string& password);
		void						removePassword();

		bool						hasTopic() const;
		const std::string&			getTopic() const;
		void						setTopic(const std::string& topic);
		bool						isTopicPrivate() const;
		void						setTopicPrivate(bool isTopicPrivate);

		bool						isInviteOnly() const;
		void						setInviteOnly(bool isInviteOnly);

		bool						hasUsersLimit() const;
		size_t						getUsersLimit() const;
		void						setUsersLimit(size_t limit);
		void						removeUsersLimit();
		bool						isFull() const;

		const std::vector<User*>&	getUsers() const;
		void						addUser(User* user);
		void						removeUser(User* user);

		const std::vector<User*>&	getOperators() const;
		void						addOperator(User* user);
		void						removeOperator(User* user);


		//broadcast message to all users in the channel

};

#endif
