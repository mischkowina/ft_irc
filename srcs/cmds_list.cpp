#include "cmds.hpp"

/////////////////////////////////// LIST ////////////////////////////////////

void	list_channel(Server *server, Client &client, Channel &channel)
{
	std::vector<std::string>	params;
	//if the channel is secret and the client is not on there, it is not displayed
	if (channel.isSecret() && channel.clientIsChannelUser(client.getNick()) == false)
		return;

	//channel name is first parameter
	if (channel.isPrivate() && channel.clientIsChannelUser(client.getNick()) == false)
		params.push_back(channel.getChannelName() + "(Prv)");
	else
		params.push_back(channel.getChannelName());
	
	//second parameter is # of visible users
	int	nbrUsers = 0;
	for (std::list<Client>::iterator it = channel.getChannelUsers().begin(); it != channel.getChannelUsers().end(); it++)
	{
		if (it->isInvisible() == false)
			nbrUsers++;
	}
	params.push_back(ft::itos(nbrUsers));
	
	//third parameter is the topic, unless the channel is private and the client is not on there
	if (channel.isPrivate() && channel.clientIsChannelUser(client.getNick()) == false)
		params.push_back("");
	else
		params.push_back(channel.getTopic());
	
	client.sendErrMsg(server, RPL_LIST, params);
}

void	list(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.empty() == true)
	{
		for (Server::ChannelMap::iterator it = server->getChannelMap().begin(); it != server->getChannelMap().end(); it++)
			list_channel(server, client, it->second);
		client.sendErrMsg(server, RPL_LISTEND, NULL);
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

	for (std::vector<std::string>::iterator itParam = channelNames.begin(); itParam != channelNames.end(); itParam++)
	{
		Server::ChannelMap::iterator itChan = server->getChannelMap().find(*itParam);
		if (itChan != server->getChannelMap().end())
			list_channel(server, client, itChan->second);
	}
	client.sendErrMsg(server, RPL_LISTEND, NULL);
}
