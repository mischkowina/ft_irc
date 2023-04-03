#include "cmds.hpp"

void findReceivers(Server *server, Client &sender, std::vector<std::string> listOfRecv, std::string msg)
{
	Server::ClientMap tmpClient = server->getClientMap();
	Server::ChannelMap tmpChannel = server->getChannelMap();

	// iterate through receivers, try to find a matching channel or client to send the message
	for (std::vector<std::string>::const_iterator itRecv = listOfRecv.begin(); itRecv != listOfRecv.end(); itRecv++)
	{
		Server::ClientMap::const_iterator itClien = tmpClient.find(*itRecv);
		if (itClien != tmpClient.end())
		{
			std::string tmp = msg;
			tmp.insert(0, (*itClien).second.getNick() + " ");
			(*itClien).second.sendPrivMsg(sender, tmp);
			continue ;
		}
		Server::ChannelMap::const_iterator itChannel = tmpChannel.find(*itRecv);
		if (itChannel != tmpChannel.end())
		{
			std::string tmp = msg;
			tmp.insert(0, (*itChannel).second.getChannelName() + " ");
			std::list<Client> tmpChannelUsers = itChannel->second.getChannelUsers();
			for (std::list<Client>::const_iterator itChannelRecv = tmpChannelUsers.begin(); itChannelRecv != tmpChannelUsers.end(); itChannelRecv++)
				(*itChannelRecv).sendPrivMsg(sender, tmp);
			continue ;
		}
		sender.sendErrMsg(server, ERR_NOSUCHNICK, (*itRecv).c_str());
	}
}

// send (private) msg to a specific user/channel
void	privmsg(Server *server, Client &client, Message& msg)
{
	// Parameters: <receiver>{,<receiver>} <text to be sent>

	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.empty() == true)
	{
		client.sendErrMsg(server, ERR_NORECIPIENT, "PRIVMSG");
		return ;
	}

	if (parameters.size() < 2)
	{
		client.sendErrMsg(server, ERR_NOTEXTTOSEND, NULL);
		return ;
	}

	std::vector<std::string>	receivers;
	std::string textToBeSend = parameters[1];

	//parse first parameter by commas to get all the recipients -> <receiver> can be a list of names or channels
	std::stringstream ss(parameters[0]);
	std::string token;
	while (std::getline(ss, token, ',')) {
		receivers.push_back(token);
		token.clear();
	}

	//send message to all receivers
	try {
		findReceivers(server, client, receivers, textToBeSend);
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;	// adjust appropriate response here
	}
}

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

void	closeChannel(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// loop through map of channels and remove client from channel when found -> else send error msg
	// check if last client; than remove channel
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

// ahhh .... check the documentation
void	connect(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// establish initial connection -> server should add this new client
	// and enable him to open/manage new channels and usage of general functions
}

void	help(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// display standard stuff ...
	std::cout << " ----> what help?\n";
}

void	info(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// display standard stuff ...

}

void	whois(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// display info about specific user
		// go through the channel's list of users
		// and return error / user's info
}

/////////////////////////////////// NICK ////////////////////////////////////

void	nick(Server *server, Client &client, Message& msg)
{
	std::vector<std::string> parameters = msg.getParameters();
	
	//check if there is at least one parameter
	if (parameters.empty() == true)
	{
		client.sendErrMsg(server, ERR_NONICKNAMEGIVEN, NULL);
		return ;
	}
	
	//check if first parameter is a valid nickname
	if (ft::isValidNick(parameters[0]) == false)
	{
		client.sendErrMsg(server, ERR_ERRONEUSNICKNAME, parameters[0].c_str());
		return ;
	}

	// try changing the nickname of the client
	// original has to be erased and a copy with the new nick to be inserted in order for map to work properly (no changing of keys)
	Client	changedClient(client);
	changedClient.setNick(parameters[0]);

	if (server->addClient(changedClient) == false)
		client.sendErrMsg(server, ERR_NICKNAMEINUSE, parameters[0].c_str());
	else
		server->eraseFromClientMap(client);
	//COMMENT: No implementation of ERR_NICKCOLLISION since it is only applicable for multi-server connections
}

/////////////////////////////////// PASS ////////////////////////////////////

void	pass_cmd(Server *server, Client &client, Message& msg)
{
	std::vector<std::string> parameters = msg.getParameters();

	//check if client is already authorized (already used PASS before)
	if (client.getIsAuthorized() == true)
	{
		client.sendErrMsg(server, ERR_ALREADYREGISTRED, NULL);
		return ;
	}
	
	//check if there is a password parameter
	if (parameters.empty() == true)
	{
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, msg.getCommand().c_str());
		return ;
	}

	//check if the password is correct, if so change status of client to isAuthorized == true
	if (parameters[0] == server->getPass())
		client.setIsAuthorized(true);
}
