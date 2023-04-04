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

void	Channel::setName(std::string channelName)
{
	_channelName = channelName;
}

void	Channel::setTopic(std::string topic)
{
	this->_topic = topic;
}

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
