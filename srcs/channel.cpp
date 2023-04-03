#include "channel.hpp"

Channel::Channel()
	: _channelbuffer("null")
	, _password("null")
	, _inviteOnly(false)
{}

Channel::~Channel() {}

Channel::Channel(std::string name)
	: _channelName(name)
	, _channelbuffer("null")
	, _password("")
	, _inviteOnly(false)
{_inviteOnly = false;}//for compiling with unused variable

std::list<Client> Channel::getChannelUsers() const
{
	return _channelUsers;
}

std::list<Client> Channel::getChannelOperator() const
{
	return _channelOperator;
}

std::string  Channel::getChannelName() const
{
	return _channelName;
}

void	Channel::setName(std::string channelName)
{
	_channelName = channelName;
}

void	Channel::setChannelOp(Client &client)
{
	_channelOperator.push_back(client);
}

void	Channel::setChannelUsers(Client &client)
{
	_channelUsers.push_back(client);
}
