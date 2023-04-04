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

	//check if channel exists
	Server::ChannelMap::iterator it = server->getChannelMap().find(parameters[0]);
	if (it == server->getChannelMap().end())
	{
		client.sendErrMsg(server, ERR_NOTONCHANNEL, parameters[0].c_str());
		return;
	}

	// if only one parameter, TOPIC is requested
	if (parameters.size() == 1)
	{
		//TO-DO: check if channel is private/secret and if so, if client is on that channel
		//TO-DOif yes (or if channel is public), return topic
		if (it->second.getTopic() != "")
		{
			std::vector<std::string>	msg_input;
			msg_input.push_back(parameters[0]);
			msg_input.push_back(it->second.getTopic());
			client.sendErrMsg(server, RPL_TOPIC, msg_input);
		}
		else
			client.sendErrMsg(server, RPL_NOTOPIC, parameters[0].c_str());
			
		//TO-DO: if no, ERR_NOTONCHANNEL
	}

	//if two parameters, topic shall be changed
	else
	{
		//TO-DO: check if only chanops can change topic and client is chanop
		it->second.setTopic(parameters[1]);
		//TO-DO: else err
	}

	//OPEN: RPL_TOPIC is send whens someone JOINs a channel
}

void	names_all(Server *server, Client &client)
{
	std::set<std::string>	nicksInChannels;
	
	for (Server::ChannelMap::iterator it = server->getChannelMap().begin(); it != server->getChannelMap().end(); it++)
	{
		
	}
}

void	names(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.empty() == true)
	{
		//TO-DO: LIST OF ALL CHANNELS AND THEIR OCCUPANTS
	}

	//parse first parameter by commas to get all channel names
	std::vector<std::string> channelNames;
	std::stringstream ss(parameters[0]);
	std::string token;
	while (std::getline(ss, token, ',')) {
		channelNames.push_back(token);
		token.clear();
	}

	for (std::vector<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); it++)
	{
		Server::ChannelMap::iterator itChan = server->getChannelMap().find(*it);
		if (itChan !=  server->getChannelMap().end())
		{
			std::vector<std::string> params;
			//TO-DO: FORMATTING according to RPL_NAMREPLY
			//RPL_NAMREPLY
		}
		client.sendErrMsg(server, RPL_ENDOFNAMES, it->c_str());
	}
}
