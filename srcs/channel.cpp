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

std::list<Client> &Channel::getChannelUsers()
{
	return _channelUsers;
}

std::set<std::string> &Channel::getChannelOperators()
{
	return _channelOperator;
}

std::set<std::string>	&Channel::getChannelBanMasks()
{
	return _banList;
}

std::string	Channel::getTopic() const
{
	return _topic;
}

std::string  Channel::getChannelName() const
{
	return _channelName;
}

std::string	Channel::getChannelKey() const
{
	return _password;
}

int	Channel::getChannelLimit() const
{
	return _userLimit;
}

std::set<std::string>	&Channel::getChannelInviteList()
{
	return _invitedUsers;
}

bool	Channel::removeUser(Client& client, std::string message, std::string type)
{
	if (message.empty())
		message = client.getNick();
	
	for (std::list<Client>::iterator it = _channelUsers.begin(); it != _channelUsers.end(); it++)
	{
		if (it->getNick() == client.getNick())
		{
			//remove nick from channel operator list as well (erase can be called for the string value in a set :-))
			_channelOperator.erase(client.getNick());
			//remove nick from voiced user list as well (erase can be called for the string value in a set :-))
			_voiceUsers.erase(client.getNick());
			// decrease userCounter
			--_userCounter;
			// decrease channelCounter
			client.decreaseChannelCounter();

			if (_quietChannel == false)
			{
				if (_anonymousChannel && message == client.getNick())
					this->sendMsgToChannel(client, "anonymous", "PART");
				else if (!_anonymousChannel && type == "QUIT")
					this->sendMsgToChannel(client, message, "QUIT");
				else
					this->sendMsgToChannel(client, message, "PART");
			}
			
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

bool	Channel::clientIsInvited(std::string nick) const
{
	if (_invitedUsers.find(nick) != _invitedUsers.end())
		return true;
	return false;
}

bool	Channel::clientIsVoicedUser(std::string nick) const
{
	if (_voiceUsers.find(nick) != _voiceUsers.end())
		return true;
	return false;
}

std::list<Client>::iterator	Channel::getChannelUser(std::string nick)
{
	for (std::list<Client>::iterator it = _channelUsers.begin(); it != _channelUsers.end(); it++)
	{
		if (it->getNick() == nick)
			return (it);
	}
	return (_channelUsers.end());
}

void	Channel::manageOperatorList(char c, std::string nick)
{
	if (c == '+')
		_channelOperator.insert(nick);
	else
		_channelOperator.erase(nick);
}

void	Channel::removeFromOperatorList(std::string nick)
{
	_channelOperator.erase(nick);
}

void	Channel::addToVoiceList(Client &client)
{
	_voiceUsers.insert(client.getNick());
}

void	Channel::addToInviteList(std::string nick)
{
	_invitedUsers.insert(nick);
}

void	Channel::removeFromInviteList(std::string nick)
{
	_invitedUsers.erase(nick);	
}

void	Channel::removeFromVoiceList(std::string nick)
{
	_voiceUsers.erase(nick);	
}

/* modes */

void	Channel::manageBanList(Server *server, Client& client, char c, std::string& banMask)
{
	if (c == '+' && banMask == "") {
		std::vector<std::string> params;
		params.push_back(_channelName);
		params.push_back("");
		if (_banList.empty())
			std::cout << "EMPTY" << std::endl;
		for (std::set<std::string>::iterator it = _banList.begin(); it != _banList.end(); it++)
		{
			params[1] = *it;
			client.sendErrMsg(server, RPL_BANLIST, params);
		}
		client.sendErrMsg(server, RPL_ENDOFBANLIST, _channelName.c_str());
	}	
	else if (c == '+') {
		std::stringstream ss(banMask);
		std::string token;
		while (std::getline(ss, token, ',')) {
			std::transform(token.begin(), token.end(), token.begin(), ::tolower);
			_banList.insert(token);
			token.clear();
		}
	}
	else if (c == '-' && banMask.size())
	{
		std::set<std::string>::iterator it = _banList.find(banMask);
		if (it != _banList.end())
			_banList.erase(it);
	}
}

void	Channel::manageVoiceList(char c, std::string& nick)
{
	if (c == '+')
		_voiceUsers.insert(nick);
	else
		_voiceUsers.erase(nick);
}

void	Channel::manageInviteList(char c, std::string& nick)
{
	if (c == '+')
		_invitedUsers.insert(nick);
	else
		_invitedUsers.erase(nick);
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

void	Channel::setLimit(char c, std::string limit)
{
	if (c == '+')
		_userLimit = atoi(limit.c_str());
	else
		_userLimit = 0;
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

void	Channel::setOutsideMsg(char c)
{
	if (c == '+')
		_noOutsideMessages = true;
	else
		_noOutsideMessages = false;
}

void	Channel::sendMsgToChannel(Client &sender, std::string message, std::string type) const
{
	for (std::list<Client>::const_iterator it = _channelUsers.begin(); it != _channelUsers.end(); it++)
	{
		std::string msg = message;
		if (it->getNick() == sender.getNick() && (type == "PRIVMSG" || type == "NOTICE"))//don't send prvmsg/notice to the sender himself
			continue;
		if (msg.find(" ", 0) != std::string::npos && type != "KICK")
			msg.insert(0, ":");
		msg.insert(0, " ");
		if (type != "NICK")
			msg.insert(0, " " + _channelName);
		//insert command name
		msg.insert(0, type);

		msg.insert(0, " ");
		//insert full prefix of sender - :<nick>!<username>@<IP> or anonymous for anonymous channels
		if (_anonymousChannel == true)
			msg.insert(0, "anonymous!anonymous@anonymous.");
		else
		{
			msg.insert(0, sender.getIP());
			msg.insert(0, "@");
			msg.insert(0, sender.getName());
			msg.insert(0, "!");
			msg.insert(0, sender.getNick());
		}
		msg.insert(0, ":");
		std::cout << BLUE "Sending to " << it->getNick() << ": " RESET << msg << std::endl;
		msg.append("\r\n");
		send(it->getSocket(), msg.data(), msg.size(), 0);
	}
}

void	Channel::updateNick(Client &oldNick, Client &newNick)
{
	std::list<Client>::iterator it = getChannelUser(oldNick.getNick());
	if (it != getChannelUsers().end())
		it->setNick(newNick.getNick());
	if (clientIsChannelOperator(oldNick.getNick()))
	{
		removeFromOperatorList(oldNick.getNick());
		manageOperatorList('+', newNick.getNick());
	}
	if (clientIsVoicedUser(oldNick.getNick()))
	{
		removeFromVoiceList(oldNick.getNick());
		addToVoiceList(newNick);
	}
	if (clientIsInvited(oldNick.getNick()))
	{
		removeFromInviteList(oldNick.getNick());
		addToInviteList(newNick.getNick());
	}
}

static bool	parseWildCards(std::string& str1, std::string& str2)
{
	if (str2[str2.size() - 1] == '*')
	{
		std::string sub1 = str2.substr(0, str2.size() - 1);
		size_t pos = str1.find(sub1);
		if (pos != std::string::npos)
			return true;
	}
	else if (str2[0] == '*')
	{
		std::string sub1 = str2.substr(1);
		size_t pos = str1.find(sub1);
		if (pos != std::string::npos)
			return true;
	}
	return false;
}

bool	Channel::includedOnBanList(Client& client) const
{
	// compare user ID against banmasks
	std::string userNick = client.getNick();
	std::string userName = client.getName();
	std::string userHost = client.getIP();

	for (std::set<std::string>::iterator it = _banList.begin(); it != _banList.end(); ++it)
	{
		std::string banNick, banUser, banHost;
		size_t pos1 = (*it).find('!');
		size_t pos2 = (*it).find('@');
		banNick = (*it).substr(0, pos1);
		banUser = (*it).substr(pos1 + 1, pos2 - pos1 - 1);
		banHost = (*it).substr(pos2 + 1);

		if (parseWildCards(userNick, banNick) == true
			&& parseWildCards(userName, banUser) == true
			&& parseWildCards(userHost, banHost) == true)
			return true;
	}
	return false;
}

void	Channel::addClientToChannel(Server *server, Client& client, std::vector<std::string> &keys, int keyIndex)
{
	// check any invalid conditions before adding a new client to the channel
		// if client has already joined the channel -> stop
	for (std::list<Client>::const_iterator it = _channelUsers.begin(); it != _channelUsers.end(); ++it) {
		if (it->getNick() == client.getNick() || it->getName() == client.getName()/* ignore while testing on localhost! || it->getIP() == client.getIP()*/)
			return;
	}
		// a client can be a member of 10 channels max
	if (client.maxNumOfChannels() == true) {
		client.sendErrMsg(server, ERR_TOOMANYCHANNELS, _channelName.c_str());
		return;
	}
		// max limit of users on a channel 
	if (_userCounter++ == _userLimit) {
		client.sendErrMsg(server, ERR_CHANNELISFULL, _channelName.c_str());
		--_userCounter;
		return;
	}
		// the correct key (password) must be given if it is set.
	if (_password != "" && (keys.size() <= (size_t)keyIndex || keys[keyIndex] != _password)) {
		client.sendErrMsg(server, ERR_BADCHANNELKEY, _channelName.c_str());
		return;
	}
		// user's nick/username/hostname must not match any active bans;
	if (includedOnBanList(client) == true) {
		client.sendErrMsg(server, ERR_BANNEDFROMCHAN, _channelName.c_str());
		return;
	}
		// if channel is invite only
	if (_inviteOnly == true && _invitedUsers.find(client.getNick()) == _invitedUsers.end()) {
		client.sendErrMsg(server, ERR_INVITEONLYCHAN, _channelName.c_str());
		return;
	}

	_channelUsers.push_back(client);
	client.increaseChannelCounter();

	// send JOIN msg to all clients on the channel
	if (_quietChannel == false)
		sendMsgToChannel(client, "", "JOIN");

	//send RPL_TOPIC && RPL_NAMREPLY && RPL_ENDOFNAMES to that client
	std::vector<std::string> params;
	params.push_back(_channelName);
	params.push_back(_topic);
	client.sendErrMsg(server, RPL_TOPIC, params);

	Message message("NAMES " + _channelName);
	names(server, client, message);
}
