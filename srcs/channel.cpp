#include "channel.hpp"

Channel::Channel()
	: _channelbuffer("null")
	, _password("")
	, _inviteOnly(false)
	, _secretChannel(false)
	, _privateChannel(false)
	, _quietChannel(false)
	, _moderatedChannel(false)
	, _supportChannelModes(true)
	, _topic("")
{}

Channel::~Channel() {}

Channel::Channel(std::string name)
	: _channelName(name)
	, _channelbuffer("null")
	, _password("")
	, _inviteOnly(false)
	, _secretChannel(false)
	, _privateChannel(false)
	, _quietChannel(false)
	, _moderatedChannel(false)
	, _supportChannelModes(true)
	, _topic("")
{
	_inviteOnly = false;	//for compiling with unused variable
	if (name.at(0) == '+')
		_supportChannelModes = false;
}

/* setters */

void	Channel::setName(std::string channelName)
{
	_channelName = channelName;
}

void	Channel::setTopic(std::string topic)
{
	this->_topic = topic;
}

void	Channel::setChannelOp(Client &client)
{
	_channelOperator.insert(client.getNick());
}

void	Channel::setChannelUsers(Client &client)
{
	_channelUsers.push_back(client);
}

/* getters */

std::list<Client> Channel::getChannelUsers() const
{
	return _channelUsers;
}

std::set<std::string> Channel::getChannelOperators() const
{
	return _channelOperator;
}

std::string	Channel::getTopic() const
{
	return _topic;
}

std::string  Channel::getChannelName() const
{
	return _channelName;
}

bool	Channel::removeUser(Client& client)
{
	for (std::list<Client>::iterator it = _channelUsers.begin(); it != _channelUsers.end(); it++)
	{
		if (it->getNick() == client.getNick())
		{
			//remove nick from channel operator list aswell (erase can be called for the string value in a set :-))
			_channelOperator.erase(client.getNick());
			//remove nick from voiced user list aswell (erase can be called for the string value in a set :-))
			_voiceUsers.erase(client.getNick());
			//remove nick from user list
			_channelUsers.erase(it);
			return true;
		}
	}
	return false;
}

bool	Channel::isInviteOnly() const
{
	return _inviteOnly;
}

bool	Channel::isSecret() const
{
	return _secretChannel;
}

bool	Channel::isPrivate() const
{
	return _privateChannel;
}

bool	Channel::isQuiet() const
{
	return _quietChannel;
}

bool	Channel::isModerated() const
{
	return _moderatedChannel;
}

bool	Channel::isTopicChangeOnlyByChanop() const
{
	return _topicChangeOnlyByChanop;
}

bool	Channel::clientIsChannelOperator(std::string nick) const
{
	std::set<std::string>::const_iterator it = _channelOperator.find(nick);
	if (it == _channelOperator.end())
		return false;
	return true;
}

bool	Channel::clientIsChannelUser(std::string nick) const
{
	std::list<Client>::const_iterator	it = _channelUsers.begin();
	while (it != _channelUsers.end() && it->getNick() != nick)
		it++;
	if (it == _channelUsers.end())
		return false;
	return true;
}


std::list<Client>::iterator	Channel::getChannelUser(std::string nick)
{
	std::list<Client>::iterator	it = _channelUsers.begin();
	while (it != _channelUsers.end() || it->getNick() != nick)
		it++;
	return (it);
}

void	Channel::addToOperatorList(Client &client)
{
	_channelOperator.insert(client.getNick());
}

void	Channel::removeFromOperatorList(std::string nick)
{
	_channelOperator.erase(nick);
}

void	Channel::addToBannedList(Client &client)
{
	_bannedUsers.push_back(client);
}

void	Channel::removeFromBannedList(std::string nick)
{
	for (std::list<Client>::iterator it = _bannedUsers.begin(); it != _bannedUsers.end(); it++)
	{
		if (it->getNick() == nick)
		{
			_bannedUsers.erase(it);
			return ;
		}
	}
}

void	Channel::addToVoiceList(Client &client)
{
	_voiceUsers.insert(client.getNick());
}

void	Channel::addToInviteList(std::string nick)
{
	_invitedUsers.insert(nick);
}

void	Channel::removeFromVoiceList(std::string nick)
{
	_voiceUsers.erase(nick);	
}

bool	Channel::supportChannelModes() const
{
	return _supportChannelModes;
}

void	Channel::setPassWD(std::string pass)
{
	_password = pass;
}

void	Channel::setInvite(bool arg)
{
	_inviteOnly = arg;
}

void	Channel::setPrivate(bool arg)
{
	// only +p or +s ??
	_privateChannel = arg;
}

void	Channel::setSecret(bool arg)
{
	_secretChannel = arg;
}

void	Channel::setTopicChangeOnlyByChanop(bool arg)
{
	_topicChangeOnlyByChanop = arg;
}

void	Channel::setModeratedChannel(bool arg)
{
	_moderatedChannel = arg;
}

void	Channel::setLimit(int limit)
{
	_userLimit = limit;
}
