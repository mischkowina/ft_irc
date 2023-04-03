#include "cmds.hpp"

bool validChannelName(Server *server, std::string name, Client &client)
{
	if ((name[0] != '#' && name[0] != '&') || name.length() > 199 || name.find_first_of(' ') != std::string::npos) {
		client.sendErrMsg(server, ERR_NAME, NULL);	// find correct macro
		return false;
	}
	return true;
}

void	splitParam(std::vector<std::string> &input, std::vector<std::string> &output, int pos)
{
	std::stringstream ss(input[pos]);
	std::string token;
	while (std::getline(ss, token, ',')) {
		output.push_back(token);
		token.clear();
	}
}

//////////////////////////////  CHANNEL  ////////////////////////////////////////
void	Server::createNewChannel(std::string name, Client &client)
{
	Channel tmp(name);
	tmp.setChannelOp(client);
	tmp.setChannelUsers(client);
	_channels.insert(std::make_pair(name, tmp));
}

void	Channel::addClientToChannel(Server *server, Client& client, std::vector<std::string> &keys, int keyIndex)
{
	// check any invalid conditions before adding a new client to the channel
		// the correct key (password) must be given if it is set.
	if (keys.empty() != false && keys[keyIndex] != _password) {
		client.sendErrMsg(server, ERR_BADCHANNELKEY, NULL);
		return;
	}
		// a client can be a member of 10 channels max
	if (client.maxNumOfChannels() == true) {
		client.sendErrMsg(server, ERR_TOOMANYCHANNELS, NULL);
		return;
	}
		// user's nick/username/hostname must not match any active bans;
	for (std::list<Client>::const_iterator it = _bannedUsers.begin(); it != _bannedUsers.end(); ++it) {
		if (it->getNick() == client.getNick() || it->getName() == client.getName() || it->getIP() == client.getIP()) {
			client.sendErrMsg(server, ERR_BANNEDFROMCHAN, NULL);
			return;
		}
	}

		// user must be invited if the channel is invite-only; -> check docs for implementation
	// std::list<Client>::const_iterator It = std::find(_invitedUsers.begin(), _invitedUsers.end(), client);
	// if (It == _invitedUsers.end()) {
	// 	client.sendErrMsg(server, ERR_INVITEONLYCHAN, NULL);
	// 	return;
	// }

	bool inv = false;
	for (std::list<Client>::const_iterator it = _invitedUsers.begin(); it != _invitedUsers.end(); ++it) {
		if (it->getNick() == client.getNick())
			inv = true;
	}
	if (_inviteOnly == true && inv == false)
	{
		client.sendErrMsg(server, ERR_INVITEONLYCHAN, NULL);
		return;
	}

	_channelUsers.push_back(client);
	// send msg to other clients
	// sendMsg();
	std::cout << client.getNick() << " joined the channel\n"; //TODO
}

void	join(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	std::vector<std::string> channelNames;
	std::vector<std::string> keys;
	splitParam(parameters, channelNames, 0);
	splitParam(parameters, keys, 1);

	int index = 0;
	Server::ChannelMap& mapOfChannels = server->getChannelMap();
	for (std::vector<std::string>::iterator iterChannelName = channelNames.begin();
		iterChannelName != channelNames.end(); iterChannelName++, index++) {

		// check channelName
		if (validChannelName(server, *iterChannelName, client) == false)
			continue;
		// find the existing channel
		Server::ChannelMap::iterator itChannel = mapOfChannels.find(*iterChannelName);	
		if (itChannel != mapOfChannels.end())
		{
			// add client to the channel
			itChannel->second.addClientToChannel(server, client, keys, index);
		}
		else {
			// else add new channel to the server
			server->createNewChannel(*iterChannelName, client);
		}
	}
}

//////////////////////////////  nick  ////////////////////////////////////////
void	names(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// display all users of a specific channel(s)
		// go through the channel's list of users
}
