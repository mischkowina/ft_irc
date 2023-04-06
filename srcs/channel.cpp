#include "channel.hpp"

Channel::Channel()
	: _channelbuffer("null")
	, _password("null")
	, _inviteOnly(false)
	, _topic("")
{}

Channel::~Channel() {}

Channel::Channel(std::string name)
	: _channelName(name)
	, _channelbuffer("null")
	, _password("")
	, _inviteOnly(false)
{_inviteOnly = false;}//for compiling with unused variable

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
	_channelOperator.push_back(client);
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

std::list<Client> Channel::getChannelOperator() const
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
			for (std::list<Client>::iterator it2 = _channelOperator.begin(); it2 != _channelOperator.end(); it2++)
			{
				if (it2->getNick() == client.getNick())
				{
					_channelOperator.erase(it2);
					break ;
				}
			}
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

bool	Channel::clientIsChannelOperator(std::string nick) const
{
	std::list<Client>::const_iterator	it = _channelOperator.begin();
	while (it != _channelOperator.end() && it->getNick() != nick)
		it++;
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

void	Channel::addClientToInviteList(std::string nick)
{
	_inviteList.insert(nick);
}

std::list<Client>::iterator	Channel::getChannelUser(std::string nick)
{
	std::list<Client>::iterator	it = _channelUsers.begin();
	while (it != _channelUsers.end() || it->getNick() != nick)
		it++;
	return (it);
}
