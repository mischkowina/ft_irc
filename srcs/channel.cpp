#include "channel.hpp"

Channel::Channel()
	: _channelName("default")
	, _password("")
	, _inviteOnly(false)
	, _secretChannel(false)
	, _privateChannel(false)
	, _quietChannel(false)
	, _moderatedChannel(false)
	, _supportChannelModes(true)
	, _topicChangeOnlyByChanop(false)
	, _anonymousChannel(false)
	, _noOutsideMessages(false)
	, _userLimit(0)
	, _userCounter(1)
	, _topic("")
{}

Channel::~Channel() {}

Channel::Channel(std::string name)
	: _channelName(name)
	, _password("")
	, _inviteOnly(false)
	, _secretChannel(false)
	, _privateChannel(false)
	, _quietChannel(false)
	, _moderatedChannel(false)
	, _supportChannelModes(true)
	, _topicChangeOnlyByChanop(false)
	, _anonymousChannel(false)
	, _noOutsideMessages(false)
	, _userLimit(0)
	, _userCounter(1)
	, _topic("")
{
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
			//remove nick from channel operator list as well (erase can be called for the string value in a set :-))
			_channelOperator.erase(client.getNick());
			//remove nick from voiced user list as well (erase can be called for the string value in a set :-))
			_voiceUsers.erase(client.getNick());
			//remove nick from user list
			_channelUsers.erase(it);
			// decrease userCounter
			--_userCounter;
			// decrease channelCounter
			client.decreaseChannelCounter();
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

bool	Channel::isAnonymous() const
{
	return _anonymousChannel;
}

bool	Channel::allowsNoOutsideMessages() const
{
	return _noOutsideMessages;
}

bool	Channel::supportedChannelModes() const
{
	return _supportChannelModes;
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

bool	Channel::clientIsVoicedUser(std::string nick) const
{
	if (_voiceUsers.find(nick) != _voiceUsers.end())
		return true;
	return false;
}

std::list<Client>::iterator	Channel::getChannelUser(std::string nick)
{
	std::list<Client>::iterator	it = _channelUsers.begin();
	while (it != _channelUsers.end() || it->getNick() != nick)
		it++;
	return (it);
}

void	Channel::manageOperatorList(char c, std::string& nick)
{
	if (c == '+')
		_channelOperator.insert(nick);
	else
		_channelOperator.erase(nick);
}

/* modes */

void	Channel::addToBannedList(Client& client, std::string list)
{
	if (list != "") {
		// parse list and include those
		// _bannedUsers.push_back(client);
		return;
	}
	for (std::list<Client>::iterator it = _bannedUsers.begin(); it != _bannedUsers.end(); ++it)
		client.sendMsg(client, it->getNick(), NULL);
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

void	Channel::setPassWD(char c, std::string pass)
{
	if (c == '+')
		_password = pass;
	else
		_password = "";
}

void	Channel::setInvite(char c)
{
	if (c == '+')
		_inviteOnly = true;
	else
		_inviteOnly = false;
}

void	Channel::setPrivate(char c)
{
	if (_secretChannel == false && c == '+')
		_privateChannel = true;
	else
		_privateChannel = false;
}

void	Channel::setSecret(char c)
{
	if (_privateChannel == false && c == '+')
		_secretChannel = true;
	else
		_secretChannel = false;
}

void	Channel::setTopicChangeOnlyByChanop(char c)
{
	if (c == '+')
		_topicChangeOnlyByChanop = true;
	else
		_topicChangeOnlyByChanop = false;
}

void	Channel::setModeratedChannel(char c)
{
	if (c == '+')
		_moderatedChannel = true;
	else
		_moderatedChannel = false;
}

void	Channel::setLimit(int limit)
{
	_userLimit = limit;
}

void	Channel::sendMsgToChannel(Client &sender, std::string msg, std::string type) const
{
	for (std::list<Client>::const_iterator it = _channelUsers.begin(); it != _channelUsers.end(); it++)
	{
		if (it->getNick() == sender.getNick())//don't send message to the sender himself
			continue;
		if (msg.find(" ", 0) != std::string::npos && type != "KICK")
			msg.insert(0, ":");
		msg.insert(0, " ");
		msg.insert(0, " " + _channelName);
		//insert command name
		msg.insert(0, type);

		msg.insert(0, " ");
		//insert full prefix of sender - :<nick>!<username>@<IP>
		msg.insert(0, sender.getIP());
		msg.insert(0, "@");
		msg.insert(0, sender.getName());
		msg.insert(0, "!");
		msg.insert(0, sender.getNick());
		msg.insert(0, ":");
		std::cout << BLUE "Sending to " << it->getNick() << ": " RESET << msg << std::endl;
		msg.append("\r\n");
		send(it->getSocket(), msg.data(), msg.size(), 0);
	}
}

void	Channel::setAnonymous(char c)
{
	if (c == '+')
		_anonymousChannel = true;
	else
		_anonymousChannel = false;
}

void	Channel::setQuiet(char c)
{
	if (c == '+')
		_quietChannel = true;
	else
		_quietChannel = false;
}

void	Channel::noOutsideMsg(char c)
{
	if (c == '+')
		_noOutsideMessages = true;
	else
		_noOutsideMessages = false;
}
