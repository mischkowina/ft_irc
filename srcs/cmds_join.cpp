#include "cmds.hpp"

//////////////////////////////  JOIN  ////////////////////////////////////////

bool	validChannelName(Server *server, std::string& name, Client &client)
{
	std::string tmp = name.substr(1);
	if ((name[0] != '#' && name[0] != '&' && name[0] != '+') || name == ""
		|| name.length() > 50 || name.find_first_of(" \a") != std::string::npos) {
		client.sendErrMsg(server, ERR_BADCHANMASK, NULL);
		return false;
	}
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	return true;
}

void	leaveChannels(Server *server, Client &client)
{
	Server::ChannelMap& mapOfChannels = server->getChannelMap();

	for (Server::ChannelMap::iterator itChan = mapOfChannels.begin(); itChan != mapOfChannels.end(); ++itChan)
	{
		itChan->second.removeUser(client, "", "PART");
		if (itChan->second.getChannelUsers().empty())
			server->removeChannel(itChan->second.getChannelName());
	}
}

void	join(Server *server, Client &client, Message& msg)
{
	std::vector<std::string> parameters = msg.getParameters();
	std::vector<std::string> channelNames;
	std::vector<std::string> keys;

	if (parameters.empty() == true) {
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, NULL);
		return;
	}
	// leave all joined channels
	if (parameters.size() == 1 && parameters[0] == "0") {
		leaveChannels(server, client);
		return;
	}
	std::stringstream ss(parameters[0]);
	std::string token;
	while (std::getline(ss, token, ',')) {
		std::transform(token.begin(), token.end(), token.begin(), ::tolower);
		channelNames.push_back(token);
		token.clear();
	}

	if (parameters.size() > 1) {
		std::stringstream ss(parameters[1]);
		while (std::getline(ss, token, ',')) {
			keys.push_back(token);
			token.clear();
		}
	}
	int index = 0;
	Server::ChannelMap& mapOfChannels = server->getChannelMap();

	for (std::vector<std::string>::iterator iterChannelName = channelNames.begin();
		iterChannelName != channelNames.end(); iterChannelName++, index++) {

		Server::ChannelMap::iterator itChannel = mapOfChannels.find(*iterChannelName);
		if (itChannel == mapOfChannels.end()) {
			// check channelName
			if (validChannelName(server, *iterChannelName, client) == false)
				continue;
			// add new channel to the server
			server->createNewChannel(*iterChannelName, client);
		}
		else {
			// add client to the channel
			itChannel->second.addClientToChannel(server, client, keys, index);
		}
	}
}
