#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>

class User;

class Channel
{
	public:
		typedef std::vector<User*>::iterator iterator;

	private:
		std::string			_name;
		bool				_hasKey;
		std::string			_key;
		bool				_isTopicRestricted;
		std::string			_topic;
		bool				_isInviteOnly;
		std::size_t			_usersLimit;
		bool				_isFull;
		std::vector<User*>	_users;
		std::vector<User*>	_operators;
		std::vector<User*>	_invitedUsers;

		Channel();
		Channel(const Channel& other);
		Channel&	operator=(const Channel& other);

	public:
		Channel(const std::string name);
		~Channel();

		bool						hasPassword() const;//
		const std::string&			getPassword() const;//
		void						setPassword(const std::string& password);//
		void						removePassword();//

		bool						hasTopic() const;//
		void						setTopic(const std::string& topic);//
		bool						getTopicRestriction() const;//
		void						setTopicRestriction(bool topicRestriction);//
		bool						isTopicRestricted() const;//

		bool						isInviteOnly() const;//
		void						setInviteOnly(bool isInviteOnly);//

		bool						hasUsersLimit() const;//
		std::size_t					getUsersLimit() const;//
		void						setUsersLimit(std::size_t limit);//
		void						removeUsersLimit();//
		bool						isFull() const;//

		const std::vector<User*>&	getUsers() const;//
		void						addUser(User* user);//
		void						removeUser(User* user);//

		const std::vector<User*>&	getOperators() const;//
		void						addOperator(User* user);//
		void						removeOperator(User* user);//

		bool						isMember(User* user) const;//
		bool						isOperator(User* user) const;//
		bool						isInvited(User* user) const;//
		std::size_t					getUserCount() const;//

		void						inviteUser(User* user);
		void						broadcastMessage(const std::string&, const User* = NULL);//
		const std::string&			getName() const;

		const std::string&			getTopic() const;//

		const std::string			getModes() const;//

		void removeInvitedUser(User*);


};

#endif
