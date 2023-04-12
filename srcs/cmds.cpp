#include "cmds.hpp"

//////////////////////////////  JOIN  ////////////////////////////////////////

bool validChannelName(Server *server, std::string name, Client &client)
{
	if ((name[0] != '#' && name[0] != '&' && name[0] != '+' && name[0] != '!') || name == ""
		|| name.length() > 50 || name.find_first_of(" \a") != std::string::npos) {
		client.sendErrMsg(server, ERR_BADCHANMASK, NULL);
		return false;
	}
	return true;
}

void	Channel::addClientToChannel(Server *server, Client& client, std::vector<std::string> &keys, int keyIndex)
{
	// check any invalid conditions before adding a new client to the channel
		// if client has already joined to the channel -> stop
	for (std::list<Client>::const_iterator it = _channelUsers.begin(); it != _channelUsers.end(); ++it) {
		if (it->getNick() == client.getNick() || it->getName() == client.getName() || it->getIP() == client.getIP()) {
			return;
		}
	}
		// the correct key (password) must be given if it is set.
	if (_password != "" && keys[keyIndex] != _password) {
		client.sendErrMsg(server, ERR_BADCHANNELKEY, NULL);
		return;
	}
		// user's nick/username/hostname must not match any active bans;
	for (std::list<Client>::const_iterator it = _bannedUsers.begin(); it != _bannedUsers.end(); ++it) {
		if (it->getNick() == client.getNick() || it->getName() == client.getName() || it->getIP() == client.getIP()) {
			client.sendErrMsg(server, ERR_BANNEDFROMCHAN, NULL);
			return;
		}
	}
		// if channel is invite only
	if (_inviteOnly == true && _invitedUsers.find(client.getNick()) == _invitedUsers.end()) {
		client.sendErrMsg(server, ERR_INVITEONLYCHAN, NULL);
		return;
	}

	_channelUsers.push_back(client);
	// send msg to other clients 		// TODO
	std::string msg = client.getNick() + " has joined the channel\n";
	for (std::list<Client>::iterator it = _channelUsers.begin(); it != _channelUsers.end(); ++it) {
		send(it->getSocket(), msg.data(), msg.length(), 0);
	}
}

void	join(Server *server, Client &client, Message& msg)
{
	std::vector<std::string> parameters = msg.getParameters();
	std::vector<std::string> channelNames;
	std::vector<std::string> keys;
/////////////////////////////////////////
	if (parameters.empty() == true) {
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, NULL);
		return;
	}
	std::stringstream ss(parameters[0]);
	std::string token;
	while (std::getline(ss, token, ',')) {
		channelNames.push_back(token);
		std::cout << "  // channelNames = " << channelNames.back() << std::endl;
		token.clear();
	}

	if (parameters.size() > 1) {
		std::stringstream ss(parameters[1]);
		while (std::getline(ss, token, ',')) {
			keys.push_back(token);
			std::cout << "  // keys = " << keys.back() << std::endl;
			token.clear();
		}
	}
/////////////////////////////////////////
	int index = 0;
	Server::ChannelMap& mapOfChannels = server->getChannelMap();
	for (std::vector<std::string>::iterator iterChannelName = channelNames.begin();
		iterChannelName != channelNames.end(); iterChannelName++, index++) {

		// check channelName
		if (validChannelName(server, *iterChannelName, client) == false)
			continue;
			// a client can be a member of 10 channels max
		if (client.maxNumOfChannels() == false) {
			client.sendErrMsg(server, ERR_TOOMANYCHANNELS, NULL);
			continue;
		}
		// find the existing channel
		Server::ChannelMap::iterator itChannel = mapOfChannels.find(*iterChannelName);
		if (itChannel != mapOfChannels.end())
		{
			// add client to the channel
			std::cout << RED "//channel location: " << &itChannel->second << " --\n";
			itChannel->second.addClientToChannel(server, client, keys, index);
		}
		else {
			// else add new channel to the server
			server->createNewChannel(*iterChannelName, client);
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
		if (itClien->IsInvisible() && channel.clientIsChannelUser(client.getNick()) == false)
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
			if (it->second.IsInvisible())
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

void	list(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.empty() == true)
	{
		for (Server::ChannelMap::iterator it = server->getChannelMap().begin(); it != server->getChannelMap().end(); it++)
		{
			std::vector<std::string>	params;
			//TO-DO: check if channel is secret or private and if client is in there 
			//TO-DO: "continue" if secret and client is not in there
			params.push_back(it->second.getChannelName());
			//TO-DO: if private or secret, push back identifier
			//TO-DO: if no, push back empty string
			params.push_back("");
			//TO-DO: if private and client not on there, push back empty string for topic
			params.push_back(it->second.getTopic());
			client.sendErrMsg(server, RPL_LIST, params);
		}
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
		{
			std::vector<std::string>	params;
			//TO-DO: check if channel is secret or private and if client is in there 
			//TO-DO: "continue" if secret and client is not in there
			params.push_back(itChan->second.getChannelName());
			//TO-DO: if private or secret, push back identifier
			//TO-DO: if no, push back empty string
			params.push_back("");
			//TO-DO: if private and client not on there, push back empty string for topic
			params.push_back(itChan->second.getTopic());
			client.sendErrMsg(server, RPL_LIST, params);
		}
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
}

/////////////////////////////////// KICK ////////////////////////////////////

void	kick_client_from_channel(Server *server, Client &client, Channel &channel, std::vector<std::string>	parameters, Client &victim)
{
	//send message with KICK information to victim
	std::string message = ":" + server->getHostname() + " KICK " + channel.getChannelName() + " ";
	if (parameters.size() > 2)
	{
		if (parameters[2].find(" ", 0) != std::string::npos)
			message.push_back(':');
		message.append(parameters[2]);
	}
	else
		message.append(client.getName());
	std::cout << RED "Sending to " << victim.getNick() << ": " RESET << message << std::endl;	
	message.append("\r\n");
	send(victim.getSocket(), message.data(), message.length(), 0);

	//call PART for the victim
	Message part_message("PART " + channel.getChannelName());
	part(server, victim, part_message);

	//send message to the whole channel that the user got kicked from

	std::list<Client> channelUsers = channel.getChannelUsers();
	for (std::list<Client>::iterator itChannelRecv = channelUsers.begin(); itChannelRecv != channel.getChannelUsers().end(); itChannelRecv++)
		itChannelRecv->sendMsg(client, channel.getChannelName() + " " + victim.getNick(), "KICK");
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
				kick_client_from_channel(server, client, channel, parameters, victim);
				return ;
			}
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
		kick_client_from_channel(server, client, channel, parameters, victim);
	}
}

void	mode(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.size() < 3) {
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, NULL);
		return;
	}
	if (client.getIsOperator() == false) {	// getChannelOperator
		client.sendErrMsg(server, ERR_NOPRIVILEGES, NULL);
		return;
	}
	std::string channel = parameters[0];
	std::string options = parameters[1];
	std::string tmp = options.substr(1);
	if ((options.at(0) != '+' && options.at(0) != '-') || tmp.length() > 3
		|| tmp.find_first_not_of("aopsitmnbvkl") != std::string::npos) {

		client.sendErrMsg(server, ERR_UNKNOWNMODE, tmp.data());
		return;
	}
	///////////////////////////////////////

	// [<limit>] [<user>] [<ban mask>]

	std::string args = parameters[2];
	int	limit = atoi(args.c_str());

	// MODE #mychannel +o userName
	Server::ChannelMap::iterator itChannel = server->getChannelMap().find(channel);

	if (itChannel->second.supportChannelModes() == false && options == "t")
	{
		/* should only exec -/+t */
		itChannel->second.setTopic(args);
		return;
	}

	// execute mode cmd
	const char* tmpCstr = tmp.c_str();
	for (unsigned int i = 0; i < tmp.length(); i++)
	{
		switch (tmpCstr[i])
		{
			case 'o':
				if (options.at(0) == '+') {
					itChannel->second.addToOperatorList(client);
				} else {
					itChannel->second.removeFromOperatorList(client.getNick());
				}
				break;
			case 'b':
				if (options.at(0) == '+') {
					itChannel->second.addToBannedList(client);
				} else {
					itChannel->second.removeFromBannedList(client.getNick());
				}
				break;
			case 'i':
				if (options.at(0) == '+') {
					itChannel->second.setInvite(true);
				} else {
					itChannel->second.setInvite(false);
				}
				break;
			case 'k':
				if (options.at(0) == '+') {
					itChannel->second.setPassWD(args);	// parse the key
				} else {
					itChannel->second.setPassWD("");
				}
				break;
			case 'p':
				if (options.at(0) == '+') {
					itChannel->second.setPrivate(true);
				} else {
					itChannel->second.setPrivate(false);
				}
				break;
			case 's':						// e.g. don't display using LIST etc.
				if (options.at(0) == '+') {
					itChannel->second.setSecret(true);
				} else {
					itChannel->second.setSecret(false);
				}
				break;
			case 't':
				if (options.at(0) == '+') {
					itChannel->second.setTopicChangeOnlyByChanop(true);
				} else {
					itChannel->second.setTopicChangeOnlyByChanop(false);
				}
				break;
			case 'm':
				if (options.at(0) == '+') {
					itChannel->second.setModeratedChannel(true);
				} else {
					itChannel->second.setModeratedChannel(false);
				}
				break;
			case 'v':
				if (options.at(0) == '+') {
					itChannel->second.addToVoiceList(client);
				} else {
					itChannel->second.removeFromVoiceList(client.getNick());
				}
				break;
			case 'l':
				if (options.at(0) == '+') {
					itChannel->second.setLimit(limit);
				} else {
					itChannel->second.setLimit(0);
				}
				break;
			case 'n':
				// run function for 'n'
				break;
			case 'q':
				// run function for 'q'
				break;
			case 'a':
				// run function for 'a'
				break;

		}
	}
}

// TODO
  // case insensitive channel names !

/*
        a - toggle the anonymous channel flag;
        n - toggle the no messages to channel from clients on the
            outside;
        q - toggle the quiet channel flag;

 */