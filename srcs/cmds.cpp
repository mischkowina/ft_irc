#include "cmds.hpp"

//////////////////////////////  CHANNEL  ////////////////////////////////////////
void	Server::createNewChannel(Server *server, std::string name, Client &client)
{
	// error.hpp -> include all needed macros
	if ((name[0] != '#' && name[0] != '&') || name.length() > 199 || name.find_first_of(' ') != std::string::npos) {
		client.sendErrMsg(server, IGN, NULL);	// replace macro
		return;
	}
	Channel tmp(name);
	tmp.setChannelOp(client);
	tmp.setChannelUsers(client);
	_channels.insert(std::make_pair(name, tmp));
}

void	Channel::addClientToChannel(Server *server, Client& client, std::vector<std::string> keys)
{
	// check any invalid conditions before adding a new client to the channel
		// the correct key (password) must be given if it is set.
	if (keys.empty() == false) {
	
	}
		// a client can be a member of 10 channels max
	if (client.maxNumOfChannels() == true) {
		client.sendErrMsg(server, ERR_TOOMANYCHANNELS, NULL);
		return;
	}
		// user's nick/username/hostname must not match any active bans;

		// the user must be invited if the channel is invite-only;
	_channelUsers.push_back(client);
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

void	join(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	std::vector<std::string> channelNames;
	std::vector<std::string> keys;
	splitParam(parameters, channelNames, 0);
	splitParam(parameters, keys, 1);

	Server::ChannelMap& mapOfChannels = server->getChannelMap();
	for (std::vector<std::string>::iterator iterChannelName = channelNames.begin();
		iterChannelName != channelNames.end(); iterChannelName++) {

		// find the existing channel
		Server::ChannelMap::iterator itChannel = mapOfChannels.find(*iterChannelName);	
		if (itChannel != mapOfChannels.end())
		{
			// add client to the channel
			itChannel->second.addClientToChannel(server, client, keys);
		}
		else {
			// else add new channel to the server
			server->createNewChannel(server, *iterChannelName, client);
		}
	}
	// send msg to channel's participants < <nick> joined the channel ...>
	std::cout << " ---- creating channels ... ----\n";
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
