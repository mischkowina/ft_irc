#include "cmds.hpp"

//////////////////////////////  CHANNEL  ////////////////////////////////////////
void	Server::createNewChannel(std::string name, Client &client)
{
	(void)client;
	// error.hpp -> include all needed macros
	if ((name[0] != '#' && name[0] != '&') || name.length() > 199 || name.find_first_of(' ') != std::string::npos) {
		// sendErrMsg();
		// return;
	}
	_channels.insert(std::make_pair(name, Channel(name)));
	// set client as op
	// _channelOperator.push_back(client);

	// set password if there's one
	// set modes (invate, private, etc.) if needed
	(void)client;
}

void	Server::addClientToChannel(Client& client)
{
	// wip
	(void)client;
	// check any invalid conditions to add a new client to the channel -> throw exception
			// the correct key (password) must be given if it is set.
			// a client can be a member of 10 channels max
			// user's nick/username/hostname must not match any active bans;
			// the user must be invited if the channel is invite-only;
}

void	join(Server *server, Client &client, Message& msg)
{
	(void)client;

	std::vector<std::string>	parameters = msg.getParameters();
	std::vector<std::string>	keys;
	// if (parameters[1].empty() == false) {
	// 	keys = parameters[1];
	// set var to later check for pass
	// }

	std::vector<std::string> channelNames;

	//parse first parameter by commas to get all channel names
	std::stringstream ss(parameters[0]);
	std::string token;
	while (std::getline(ss, token, ',')) {
		channelNames.push_back(token); // or std::pair<std::string, bool>(token, validName)
		token.clear();
	}

	Server::ChannelMap mapOfChannels = server->getChannelMap();			// channel -> <#name, Channel()>
	for (std::vector<std::string>::const_iterator iterChannelName = channelNames.begin();
		iterChannelName != channelNames.end(); iterChannelName++) {

		// find the existing channel
		Server::ChannelMap::const_iterator itChannel = mapOfChannels.find(*iterChannelName);	
		if (itChannel != mapOfChannels.end())
		{
			// add client to the channel
			server->addClientToChannel(client);
		}
	
		else {
			// else add new channel to the server
			server->createNewChannel(*iterChannelName, client);
		}
	}
	// send msg to channel's participants < <nick> joined the channel ...>
	std::cout << " ---- creating channels ... ----\n";
}

void	part(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.empty() == true)
	{
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, "PART");
		return ;
	}
	
	std::vector<std::string> channelNames;

	//parse first parameter by commas to get all channel names
	std::stringstream ss(parameters[0]);
	std::string token;
	while (std::getline(ss, token, ',')) {
		channelNames.push_back(token);
		token.clear();
	}

	Server::ChannelMap::iterator it;
	for (size_t i = 0; i < channelNames.size(); i++)
	{
		it = server->getChannelMap().find(channelNames[i]);
		if (it != server->getChannelMap().end())
		{
			if (it->second.removeUser(client) == false)
				client.sendErrMsg(server, ERR_NOTONCHANNEL, channelNames[i].c_str());
		}
		else 
			client.sendErrMsg(server, ERR_NOSUCHCHANNEL, channelNames[i].c_str());
	}
}

void	topic(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.empty() == true)
	{
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, "TOPIC");
		return ;
	}

	// if only one parameter, TOPIC is requested
	if (parameters.size() == 1)
	{
		//check if channel exists
		//else ERR_NOSUCHCHANNEL????
		//check if channel is private/secret and if so, if client is on that channel
		// if yes (or if channel is public), return topic
		// if no, ERR_NOTONCHANNEL
	}
	
	//if two parameters, topic shall be changed
	//ERR: check if only chanops can change topic and client is chanop

	//OPEN: RPL_TOPIC is send whens someone JOINs a channel
}

void	names(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// display all users of a specific channel(s)
		// go through the channel's list of users
}
