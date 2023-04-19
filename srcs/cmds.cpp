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

bool	parseWildCards(std::string& str1, std::string& str2)
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

bool	Channel::includedOnBanList(Server *server, Client& client)
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
		{
			client.sendErrMsg(server, ERR_BANNEDFROMCHAN, NULL);
			return false;
		}
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
		client.sendErrMsg(server, ERR_TOOMANYCHANNELS, NULL);
		return;
	}
		// max limit of users on a channel 
	if (_userCounter++ == _userLimit) {
		client.sendErrMsg(server, ERR_CHANNELISFULL, NULL);
		--_userCounter;
		return;
	}
		// the correct key (password) must be given if it is set.
	if (_password != "" && keys[keyIndex] != _password) {
		client.sendErrMsg(server, ERR_BADCHANNELKEY, NULL);
		return;
	}
		// user's nick/username/hostname must not match any active bans;
	if (includedOnBanList(server, client) == true) {
		client.sendErrMsg(server, ERR_BANNEDFROMCHAN, NULL);
		return;
	}
		// if channel is invite only
	if (_inviteOnly == true && _invitedUsers.find(client.getNick()) == _invitedUsers.end()) {
		client.sendErrMsg(server, ERR_INVITEONLYCHAN, NULL);
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

void	leaveChannels(Server *server, Client &client)
{
	Server::ChannelMap& mapOfChannels = server->getChannelMap();

	for (Server::ChannelMap::iterator itChan = mapOfChannels.begin(); itChan != mapOfChannels.end(); ++itChan)
		itChan->second.removeUser(client);
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

//////////////////////////////  PART  ////////////////////////////////////////

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

	std::string	message;
	if (parameters.size() > 1)
		message = parameters[1];
	else 
		message = client.getNick();

	Server::ChannelMap::iterator it;
	for (size_t i = 0; i < channelNames.size(); i++)
	{
		it = server->getChannelMap().find(channelNames[i]);
		if (it != server->getChannelMap().end())
		{
			if (it->second.removeUser(client) == false)
				client.sendErrMsg(server, ERR_NOTONCHANNEL, channelNames[i].c_str());
			else
			{
				if (it->second.isQuiet())
					continue ;
				if (it->second.isAnonymous() && message == client.getNick())
					it->second.sendMsgToChannel(client, "anonymous", "PART");
				else
					it->second.sendMsgToChannel(client, message, "PART");
			}	
		}
		else 
			client.sendErrMsg(server, ERR_NOSUCHCHANNEL, channelNames[i].c_str());
	}
}

/////////////////////////////////// TOPIC ////////////////////////////////////

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
		//if channel is private/secret and client is not on, return ERR_NOTONCHANNEL
		if ((it->second.isPrivate() || it->second.isSecret()) && it->second.clientIsChannelUser(client.getNick()) == false)
		{
			client.sendErrMsg(server, ERR_NOTONCHANNEL, parameters[0].c_str());
			return ;
		}
		//if there is a topic, return RPL_TOPIC
		if (it->second.getTopic() != "")
		{
			std::vector<std::string>	msg_input;
			msg_input.push_back(parameters[0]);
			msg_input.push_back(it->second.getTopic());
			client.sendErrMsg(server, RPL_TOPIC, msg_input);
		}
		//else return RPL_NOTOPIC
		else
			client.sendErrMsg(server, RPL_NOTOPIC, parameters[0].c_str());
	}

	//if two parameters, topic shall be changed
	else
	{
		//can only change topic if on the channel
		if (it->second.clientIsChannelUser(client.getNick()) == false)
		{
			client.sendErrMsg(server, ERR_NOTONCHANNEL, parameters[0].c_str());
			return ;
		}
		//if channelmode +t, only chanops can change the topic
		if (it->second.isTopicChangeOnlyByChanop() && it->second.clientIsChannelOperator(client.getNick()) == false)
		{
			client.sendErrMsg(server, ERR_CHANOPRIVSNEEDED, it->second.getChannelName().c_str());
			return ;
		}
		it->second.setTopic(parameters[1]);
	}
}

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

/////////////////////////////////// LIST ////////////////////////////////////

void	list_channel(Server *server, Client &client, Channel &channel)
{
	std::vector<std::string>	params;
	//if the channel is secret and the client is not on there, it is not displayed
	if (channel.isSecret() && channel.clientIsChannelUser(client.getNick()) == false)
		return;

	//channel name is first parameter
	params.push_back(channel.getChannelName());
	
	//second parameter is "Prv" if the channel is private and the client is not on there - else empty
	if (channel.isPrivate() && channel.clientIsChannelUser(client.getNick()) == false)
		params.push_back("Prv");
	else
		params.push_back("");
	
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

/////////////////////////////////// INVITE ////////////////////////////////////

void	invite(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();
	Server::ClientMap			clients = server->getAuthorizedClientMap();
	Server::ChannelMap			channels = server->getChannelMap();

	//check if enough parameters
	if (parameters.size() < 2)
	{
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, "INVITE");
		return ;
	}

	//check if the client specified by the first parameter exists
	Server::ClientMap::iterator receiver = clients.find(parameters[0]);
	if (receiver == clients.end())
	{
		client.sendErrMsg(server, ERR_NOSUCHNICK, parameters[0].c_str());
		return ;
	}

	//check if the channel exists and if so, if client is on the channel 
	Server::ChannelMap::iterator itChan = channels.find(parameters[1]);
	if (itChan == channels.end() || itChan->second.clientIsChannelUser(client.getNick()) == false)
	{
		client.sendErrMsg(server, ERR_NOTONCHANNEL, parameters[1].c_str());
		return ;
	}

	//check if channel is invite only and if so, if client has chanop priviliges
	if (itChan->second.isInviteOnly() == true && itChan->second.clientIsChannelOperator(client.getNick()) == false)
	{
		client.sendErrMsg(server, ERR_CHANOPRIVSNEEDED, parameters[1].c_str());
		return ;
	}

	//check if the invited client is already on that channel
	if (itChan->second.clientIsChannelUser(parameters[0]) == true)
	{
		std::vector<std::string> params;
		params.push_back(parameters[0]);
		params.push_back(parameters[1]);
		client.sendErrMsg(server, ERR_USERONCHANNEL, params);
		return ;
	}

	//add the name of the invited client to the invite list of the channel - to be checked for the nick while joining when the channel is invite only
	itChan->second.addToInviteList(parameters[0]);

	//send message to client that has been invited
	receiver->second.sendMsg(client, parameters[1], "INVITE");

	//send reply to the client that invited to signal succesful invitation
	std::vector<std::string> params;
	params.push_back(parameters[1]);
	params.push_back(parameters[0]);
	client.sendErrMsg(server, RPL_INVITING, params);
	if (receiver->second.getAwayMsg().empty() == false)
	{
		std::vector<std::string> params;
		params.push_back(receiver->second.getNick());
		params.push_back(receiver->second.getAwayMsg());
		client.sendErrMsg(server, RPL_AWAY, params);
	}
}

/////////////////////////////////// KICK ////////////////////////////////////

void	kick_client_from_channel(Client &client, Channel &channel, std::vector<std::string>	parameters, Client &victim)
{
	std::string	msg = victim.getNick() + " :";

	//KICK message contains the victims name + the reason for getting kicked (if given) or the name of the person that kicked
	if (parameters.size() > 2)
		msg.append(parameters[2]);
	else
		msg.append(client.getNick());
	
	//send message to the whole channel that the user got kicked from (still including that user) - unless it's a quiet channel
	if (channel.isQuiet() == false)
		channel.sendMsgToChannel(client, msg, "KICK");

	//actually remove the victim from the channel
	channel.removeUser(victim);
}

void	kick(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();
	
	if (parameters.size() < 2)
	{
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, "KICK");
		return ;
	}

	std::vector<std::string> channelNames;
	std::stringstream ss(parameters[0]);
	std::string token;
	while (std::getline(ss, token, ',')) {
		channelNames.push_back(token);
		token.clear();
	}

	std::vector<std::string> clientNames;
	std::stringstream ss2(parameters[1]);
	std::string token2;
	while (std::getline(ss2, token2, ',')) {
		clientNames.push_back(token2);
		token2.clear();
	}

	//either there is only one channel name with multiple users to be kicked, or one channel name per user name
	if (channelNames.size() > 1 && channelNames.size() != clientNames.size())
	{
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, "KICK");
		return ;
	}
	
	//go through all channels and remove the respective clients
	for (size_t i = 0; i < channelNames.size(); i++)
	{
		//check if channel exists
		if (server->getChannelMap().find(channelNames[i]) == server->getChannelMap().end())
		{
			client.sendErrMsg(server, ERR_NOSUCHCHANNEL, channelNames[i].c_str());
			continue;
		}

		//check if client is on that channel
		Channel channel = server->getChannelMap().find(channelNames[i])->second;
		if (channel.clientIsChannelUser(client.getNick()) == false)
		{
			client.sendErrMsg(server, ERR_NOTONCHANNEL, channelNames[i].c_str());
			continue;
		}

		//check if client is chanop and allowed to kick others
		if (channel.clientIsChannelOperator(client.getNick()) == false)
		{
			client.sendErrMsg(server, ERR_CHANOPRIVSNEEDED, channelNames[i].c_str());
			continue;
		}

		//check if one or more users are supposed to be kicked
		if (channelNames.size() == 1 && clientNames.size() > 1)
		{
			for (size_t j = 0; j < clientNames.size(); j++)
			{
				//check if the user to be kicked is in the channel
				if (channel.clientIsChannelUser(clientNames[j]) == false)
				{
					std::vector<std::string> params;
					params.push_back(clientNames[j]);
					params.push_back(channel.getChannelName());
					client.sendErrMsg(server, ERR_USERNOTINCHANNEL, params);
					continue;
				}

				//send message with KICK information to victim
				Client victim = *(channel.getChannelUser(clientNames[j]));
				kick_client_from_channel(client, channel, parameters, victim);
			}
			return ;
		}

		//else kick out one user each from multiple channels

		//check if the user to be kicked is in the channel
		if (channel.clientIsChannelUser(clientNames[i]) == false)
		{
			std::vector<std::string> params;
			params.push_back(clientNames[i]);
			params.push_back(channel.getChannelName());
			client.sendErrMsg(server, ERR_USERNOTINCHANNEL, params);
			continue;
		}

		//send message with KICK information to victim
		Client victim = *(channel.getChannelUser(clientNames[i]));
		kick_client_from_channel(client, channel, parameters, victim);
	}
}

/////////////////////////////////// MODE ////////////////////////////////////

void	info_all_user_modes(Server *server, Client &client)
{
	std::string	modes = "";

	if (client.isInvisible())
		modes += "i";
	if (client.getIsOperator())
		modes += "o";
	if (modes.empty() == false)
		modes.insert(0, "+");
	client.sendErrMsg(server, RPL_UMODEIS, modes.c_str());
}

void	info_all_channel_modes(Server *server, Client &client, Channel &channel)
{
	std::vector<std::string> params;
	params.push_back(channel.getChannelName());
	params.push_back("");
	params.push_back("");

	//if channel is anonymous
	if (channel.isAnonymous())
	{
		params[1] = "+a";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is n - no messages from outside
	if (channel.allowsNoOutsideMessages())
	{
		params[1] = "+n";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is quiet
	if (channel.isQuiet())
	{
		params[1] = "+q";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is secret
	if (channel.isSecret())
	{
		params[1] = "+s";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is private
	if (channel.isPrivate())
	{
		params[1] = "+p";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is moderated
	if (channel.isModerated())
	{
		params[1] = "+m";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is settable by operator only
	if (channel.isTopicChangeOnlyByChanop())
	{
		params[1] = "+t";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is invite
	if (channel.isInviteOnly())
	{
		params[1] = "+i";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel has a password
	if (channel.getChannelKey().empty() == false)
	{
		params[1] = "+k";
		params[2] = channel.getChannelKey() + " ";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is limited
	if (channel.getChannelLimit() > 0)
	{
		params[1] = "+l";
		std::stringstream ss;
		ss << channel.getChannelLimit();
		params[2] = ss.str();
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel has ban mask - TODO
	// if (channel.getChannelBanMasks().empty() == false)
	// {
	// 	params.pop_back();
	// 	for (std::vector<std::string>::iterator it = channel.getChannelBanMasks().begin(); it != channel.getChannelBanMasks().end(); it++)
	// 	{
	// 		params[1] = *it;
	// 		client.sendErrMsg(server, RPL_BANLIST, params);
	// 	}
	// 	client.sendErrMsg(server, RPL_BANLIST, channel.getChannelName().c_str());	
	// }
}

void	userMode(Server *server, Client &client, std::vector<std::string>& parameters)
{
	//if first parameters is a nickname and it is not the same as the client issuing the message, send ERR msg -> clients are not allowed to change or view user modes for anyone but themselves
	if (parameters[0] != client.getNick()) {
		client.sendErrMsg(server, ERR_USERSDONTMATCH, NULL);
		return;
	}
	
	//if there is no second parameter, it's a query to see all active user modes
	if (parameters.size() < 2)
	{
		info_all_user_modes(server, client);
		return;
	}

	//check if second parameter is valid
	std::string options = parameters[1];
	std::string flags = options.substr(1);
	if ((options[0] != '+' && options[0] != '-') || flags.length() > 3 || flags.find_first_not_of("oi") != std::string::npos)
	{
		client.sendErrMsg(server, ERR_UMODEUNKNOWNFLAG, NULL);
		return;
	}

	//change stati for all flags in the second parameter
	for (size_t i = 0; i < flags.length(); i++)
	{
		bool status;
		if (options[0] == '+')
			status = true;
		else
			status = false;
		if (flags[i] == 'i')
		{
			if (client.setIsInvisible(status))
			{
				//set invisibility status in all channels of that user in case it changed
				for (Server::ChannelMap::iterator it = server->getChannelMap().begin(); it != server->getChannelMap().end(); it++)
				{
					if (it->second.clientIsChannelUser(client.getNick()))
						it->second.getChannelUser(client.getNick())->setIsInvisible(status);
				}
			}
		}
		if (flags[i] == 'o' && status == false)
			client.setIsOperator(false);
	}
	client.sendMsg(client, options, "MODE");
}

void	mode(Server *server, Client &client, Message& msg)
{
	std::vector<std::string> parameters = msg.getParameters();

	if (parameters.empty()) {
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, NULL);
		return;
	}

	//check if parameter[0] is a channel or a user - if user, call usermode function, if none send error, else continue with this function
	if (server->getAuthorizedClientMap().find(parameters[0]) != server->getAuthorizedClientMap().end())
	{
		userMode(server, client, parameters);
		return;
	}
	else if (server->getChannelMap().find(parameters[0]) == server->getChannelMap().end())
	{
		if (parameters[0].find_first_of("+#&") != std::string::npos)
			client.sendErrMsg(server, ERR_NOSUCHCHANNEL, parameters[0].c_str());
		else
			client.sendErrMsg(server, ERR_NOSUCHNICK, parameters[0].c_str());
	}

	//if there is only one (valid) parameter, send info message on all modes of the channel
	if (parameters.size() < 2) 
		info_all_channel_modes(server, client, server->getChannelMap().find(parameters[0])->second);

	std::string	channel = parameters[0];
	std::transform(channel.begin(), channel.end(), channel.begin(), ::tolower);

	Server::ChannelMap::iterator itChannel = server->getChannelMap().find(channel);

		// MODE <channel> <flags> [<args>]
	std::set<std::string> operators = itChannel->second.getChannelOperators();
	std::string options = parameters[1];
	std::string flags = options.substr(1);
	if ((options[0] != '+' && options[0] != '-') || flags.length() > 3 || flags.find_first_not_of("aopsitqmnbvkl") != std::string::npos)
	{ 
		client.sendErrMsg(server, ERR_UNKNOWNMODE, flags.data());
		return;
	}
	if (operators.find(client.getNick()) == operators.end()) {
		client.sendErrMsg(server, ERR_NOPRIVILEGES, NULL);
		return;
	}
	if (itChannel->second.supportedChannelModes() == false && flags == "t") {
		itChannel->second.setTopicChangeOnlyByChanop(options[0]);
		return;
	}

	///////////////////////////////////////

	// +vbl [<limit>] [<user>] [<ban mask>]
	std::string limit;
	std::string user;
	std::string addBanList;

	if (parameters.size() > 2)
	{
		std::stringstream ss(parameters[2]);
		std::string token;
		if (flags.find_first_of('l') != std::string::npos){
			std::getline(ss, token, ' ');
			limit = token;
			token.clear();
		}
		if (ss.good() && flags.find('l') == std::string::npos){
			std::getline(ss, token, ' ');
			user = token;
			token.clear();
		}
		if (ss.good() && flags.find_first_of('b') != std::string::npos){
			std::getline(ss, token, ' ');
			token.clear();
		}
	}

	const char* tmpCStr = flags.c_str();
	for (unsigned int i = 0; i < flags.length(); i++)
	{
		//check for o / v option if given user exists / is on the channel
		if (tmpCStr[i] == 'o' || tmpCStr[i] == 'v')
		{
			//check if user exists on the server
			if (server->getAuthorizedClientMap().find(user) == server->getAuthorizedClientMap().end())
			{
				client.sendErrMsg(server, ERR_NOSUCHNICK, user.c_str());
				continue ;
			}
			//check if user is on the channel
			if (itChannel->second.clientIsChannelUser(user) == false)
			{
				std::vector<std::string> params;
				params.push_back(user);
				params.push_back(itChannel->second.getChannelName());
				client.sendErrMsg(server, ERR_USERNOTINCHANNEL, params);
				continue ;
			}
		}
		
		std::string message;
		message.push_back(options[0]);
		message.push_back(tmpCStr[i]);

		switch (tmpCStr[i])
		{
			case 'o':
				itChannel->second.manageOperatorList(options[0], user);
				message.append(" " + user);
				break;
			case 'b':
				itChannel->second.manageBanList(client, options[0], addBanList);
				message.append(" " + addBanList);
				break;
			case 'v':
				itChannel->second.manageVoiceList(options[0], user);
				message.append(" " + user);
				break;
			case 'k':
				itChannel->second.setPassWD(options[0], user);
				message.append(" " + user);
				break;
			case 'i':
				itChannel->second.setInvite(options[0]);
				break;
			case 'p':
				itChannel->second.setPrivate(options[0]);
				break;
			case 's':
				itChannel->second.setSecret(options[0]);
				break;
			case 't':
				itChannel->second.setTopicChangeOnlyByChanop(options[0]);
				break;
			case 'm':
				itChannel->second.setModeratedChannel(options[0]);
				break;
			case 'l':
				itChannel->second.setLimit(options[0], limit);
				message.append(" " + limit);
				break;
			case 'n':
				itChannel->second.setOutsideMsg(options[0]);
				break;
			case 'q':
				itChannel->second.setQuiet(options[0]);
				break;
			case 'a':
				itChannel->second.setAnonymous(options[0]);
				break;
		}
		itChannel->second.sendMsgToChannel(client, message, "MODE");
	}
}
