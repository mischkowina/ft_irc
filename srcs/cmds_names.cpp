#include "cmds.hpp"

/////////////////////////////////// NAMES ////////////////////////////////////

void	names_per_channel(Server *server, Client &client, Channel &channel)
{
	//if it's a secret or a private channel that the client is not on, it is not displayed
	if ((channel.isSecret() || channel.isPrivate()) && channel.clientIsChannelUser(client.getNick()) == false)
		return;
	
	std::vector<std::string>	params;
	std::string					param1 = channel.getChannelName();
	std::string					param2 = "";
	std::list<Client>			channelClients = channel.getChannelUsers();

	//channel name is first parameters for RPL_NAMREPLY
	if (channel.isSecret())
		param1.insert(0, "@ ");
	else if (channel.isPrivate())
		param1.insert(0, "* ");
	else
		param1.insert(0, "= ");
	params.push_back(param1);

	//second parameter gets filled with list of clients on that channel
	for (std::list<Client>::iterator itClien = channelClients.begin(); itClien != channelClients.end(); itClien++)
	{
		//if the respective user is invisible and the client is not on the same channel, it doesn't get displayed
		if (itClien->isInvisible() && channel.clientIsChannelUser(client.getNick()) == false)
			continue;
		//if the channel is Anonymous, only the name of the active client is shown (if he is on the channel)
		if (channel.isAnonymous() && itClien->getNick() != client.getNick())
			continue;
		//separate nicknames by space
		if (param2.empty() == false)
			param2.push_back(' ');
		//if the user is a chanop or a voiced user in a moderated channel, it gets a prefix
		if (channel.clientIsChannelOperator(itClien->getNick()))
			param2.push_back('@');
		else if (channel.isModerated() && channel.clientIsVoicedUser(itClien->getNick()))
			param2.push_back('+');
		param2.append(itClien->getNick());
	}
	params.push_back(param2);
	client.sendErrMsg(server, RPL_NAMREPLY, params);
	client.sendErrMsg(server, RPL_ENDOFNAMES, channel.getChannelName().c_str());
}

void	names_all(Server *server, Client &client)
{
	std::set<std::string>	nicksInChannels;

	for (Server::ChannelMap::iterator itChan = server->getChannelMap().begin(); itChan != server->getChannelMap().end(); itChan++)
	{
		names_per_channel(server, client, itChan->second);
		std::list<Client>			channelClients = itChan->second.getChannelUsers();
		for (std::list<Client>::iterator itClien = channelClients.begin(); itClien != channelClients.end(); itClien++)
			nicksInChannels.insert(itClien->getNick());
	}

	std::vector<std::string> params;
	params.push_back("*");
	std::string	param2 = "";
	for (Server::ClientMap::iterator it = server->getAuthorizedClientMap().begin(); it != server->getAuthorizedClientMap().end(); it++)
	{
		if (nicksInChannels.find(it->second.getNick()) == nicksInChannels.end())
		{
			if (it->second.isInvisible())
				continue; 
			if (param2.empty() == false)
				param2.push_back(' ');
			param2.append(it->second.getNick());
		}
	}
	if (param2.empty() == false)
	{
		params.push_back(param2);
		client.sendErrMsg(server, RPL_NAMREPLY, params);
		client.sendErrMsg(server, RPL_ENDOFNAMES, "*");
	}
}

void	names(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	//if no parameters, list every channel
	if (parameters.empty() == true)
	{
		names_all(server, client);
		return ;
	}

	//parse first parameter by commas to get all channel names
	std::vector<std::string> channelNames;
	std::stringstream ss(parameters[0]);
	std::string token;
	while (std::getline(ss, token, ',')) {
		std::transform(token.begin(), token.end(), token.begin(), ::tolower);
		channelNames.push_back(token);
		token.clear();
	}

	for (std::vector<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); it++)
	{
		Server::ChannelMap::iterator itChan = server->getChannelMap().find(*it);
		if (itChan !=  server->getChannelMap().end())
			names_per_channel(server, client, itChan->second);
	}
}
