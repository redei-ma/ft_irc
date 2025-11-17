#include "Channel.hpp"

#include <vector>
#include <string>
#include <algorithm>
#include "User.hpp"

Channel::Channel(const std::string& name)
	: _name(name),
	  _hasKey(false), _key(""),
	  _isTopicRestricted(false), _topic(""),
	  _isInviteOnly(false),
	  _usersLimit(0), _isFull(false)
	  {}

Channel::~Channel() {}

const std::string& Channel::getName() const { return _name; }

bool Channel::hasPassword() const { return _hasKey; }

const std::string& Channel::getPassword() const { return _key; }

void Channel::setPassword(const std::string& password)
{
	_key = password;
	_hasKey = true;
}

void Channel::removePassword()
{
	_key.clear();
	_hasKey = false;
}

bool Channel::hasTopic() const { return !_topic.empty(); }

const std::string& Channel::getTopic() const { return _topic; }

void Channel::setTopic(const std::string& topic) { _topic = topic; }

bool Channel::getTopicRestriction() const { return _isTopicRestricted; }

void Channel::setTopicRestriction(bool topicRestriction) { _isTopicRestricted = topicRestriction; }

bool Channel::isTopicRestricted() const { return _isTopicRestricted; }

bool Channel::isInviteOnly() const { return _isInviteOnly; }

void Channel::setInviteOnly(bool isInviteOnly) { _isInviteOnly = isInviteOnly; }

bool Channel::hasUsersLimit() const { return _usersLimit > 0; }

std::size_t Channel::getUsersLimit() const { return _usersLimit; }

void Channel::setUsersLimit(std::size_t limit)
{
	_usersLimit = limit;
	if (_users.size() >= _usersLimit)
		_isFull = true;
}

void	Channel::inviteUser(User* user)
{
	_invitedUsers.push_back(user);
}

void Channel::removeUsersLimit()
{
	_usersLimit = 0;
	_isFull = false;
}

bool Channel::isFull() const { return _isFull; }

const std::vector<User*>& Channel::getUsers() const { return _users; }

void Channel::addUser(User* user)
{
	_users.push_back(user);
	if (hasUsersLimit() && _users.size() >= _usersLimit)
		_isFull = true;
	removeInvitedUser(user);
}

void Channel::removeInvitedUser(User* user)
{
	iterator it = std::find(_invitedUsers.begin(), _invitedUsers.end(), user);
	if (it != _invitedUsers.end())
		_invitedUsers.erase(it);
}

void Channel::removeUser(User* user)
{
	removeOperator(user);
	removeInvitedUser(user);
	iterator it = std::find(_users.begin(), _users.end(), user);
	if (it != _users.end())
		_users.erase(it);
	if (hasUsersLimit() && _users.size() < _usersLimit)
		_isFull = false;
}

const std::vector<User*>& Channel::getOperators() const { return _operators; }

void Channel::addOperator(User* user) { _operators.push_back(user); }

void Channel::removeOperator(User* user)
{
	iterator it = std::find(_operators.begin(), _operators.end(), user);
	if (it != _operators.end())
		_operators.erase(it);
}

bool Channel::isMember(User* user) const { return std::find(_users.begin(), _users.end(), user) != _users.end(); }

bool Channel::isOperator(User* user) const { return std::find(_operators.begin(), _operators.end(), user) != _operators.end(); }

bool Channel::isInvited(User* user) const { return std::find(_invitedUsers.begin(), _invitedUsers.end(), user) != _invitedUsers.end(); }

std::size_t Channel::getUserCount() const { return _users.size(); }

void Channel::broadcastMessage(const std::string& message, User* sender)
{
	for (iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (*it != sender)
			(*it)->sendMessage(message);
	}
}

