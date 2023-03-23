#include "cmds.hpp"

void findReceivers(Server *server, std::vector<std::string> listOfRecv, std::string msg)
{
	// bool found = false;
	Server::ClientMap tmpClient = server->getClientMap();
	Server::ChannelMap tmpChannel = server->getChannelMap();

	// iterate through server's clients

	for (std::vector<std::string>::const_iterator itRecv = listOfRecv.begin(); itRecv != listOfRecv.end(); itRecv++)
	{
		Server::ClientMap::const_iterator itClien = tmpClient.find(*itRecv);
		if (itClien != tmpClient.end())
		{
			//OPEN: NEEDS FORMATING
			send(itClien->second.getSocket(), msg.data(), msg.size(), 0);
			continue ;
		}
		Server::ChannelMap::const_iterator itChannel = tmpChannel.find(*itRecv);
		if (itChannel != tmpChannel.end())
		{
			std::list<Client> tmpChannelUsers = itChannel->second.getChannelUsers();
			for (std::list<Client>::const_iterator itChannelRecv = tmpChannelUsers.begin(); itChannelRecv != tmpChannelUsers.end(); itChannelRecv++)
			{
				//OPEN: NEEDS FORMATING
				send(itChannelRecv->getSocket(), msg.data(), msg.size(), 0);
			}
			continue ;
		}
		//ERROR: receiver not found
	}

	
	// for (ft::ClientMap::const_iterator itClien = tmpClient.begin(); itClien != tmpClient.end(); ++itClien)
	// {
	// 	std::cout << "Clients TEST: " <<itClien->second.getNick() << std::endl;
	// 	for (std::vector<std::string>::const_iterator itRecv = listOfRecv.begin(); itRecv != listOfRecv.end(); itRecv++) {
	// 		if (itClien->second.getNick() == *itRecv)
	// 		{
	// 			send(itClien->second.getSocket(), msg.data(), msg.size(), 0);
	// 			found = true;
	// 		}
	// 	}
	// }

	// ft::ChannelMap tmpChannel = server->getChannelMap();
	// Channel 	dummyChannel("dummy");
	// tmpChannel.insert(std::make_pair("dummy", dummyChannel));
	// // iterate through channels' clients
	// for (ft::ChannelMap::const_iterator itChannel = tmpChannel.begin(); itChannel != tmpChannel.end(); ++itChannel)
	// {
	// 	std::cout << "Channel TEST: " <<itChannel->second.getChannelName() << std::endl;
	// 	for (std::vector<std::string>::const_iterator itRecv = listOfRecv.begin(); itRecv != listOfRecv.end(); itRecv++) {
	// 		if (itChannel->second.getChannelName() == *itRecv)
	// 		{
	// 			// iterate through channel's list of users
	// 			std::list<Client> tmpChannelUsers = itChannel->second.getChannelUsers();
	// 			for (std::list<Client>::const_iterator itChannelRecv = tmpChannelUsers.begin(); itChannelRecv != tmpChannelUsers.end(); itChannelRecv++) {
	// 				send(itChannelRecv->getSocket(), msg.data(), msg.size(), 0);
	// 			}
	// 			found = true;
	// 		}
	// 	}
	// }

	// if (found == false)
	// {
	// 	// return msg to client ? check irc docs
	// 	throw std::runtime_error("no receivers found");
	// }
}

// send (private) msg to a specific user/channel
void	privmsg(Server *server, Client &client, Message& msg)
{
	// (void)server;
	(void)client;
	// (void)msg;

	//parse first parameter by commas to get all the recipients -> <receiver> can be a list of names or channels
	// Parameters: <receiver>{,<receiver>} <text to be sent>

	// parse parameters inside of each function seperately 
	std::vector<std::string>	parameters = msg.getParameters();
	std::vector<std::string>	receivers;
	std::string textToBeSend = parameters[1];

	std::stringstream ss(parameters[0]);
	std::string token;
	while (std::getline(ss, token, ',')) {
		receivers.push_back(token);
		token.clear();
	}

	// std::cout << "TEST1: " << textToBeSend << std::endl;
	// std::cout << "TEST2: " << std::endl;
	// for (std::vector<std::string>::iterator it = receivers.begin(); it != receivers.end(); it++)
	// 	std::cout << *it << std::endl;

	try {
		findReceivers(server, receivers, textToBeSend);
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;	// adjust appropriate response here
	}

	//send the message (2nd parameter) to all receivers
	// --> get the socket descriptor of each receiver and send the message
	// client->_name: 2nd parameter
	// e.g.: Primoz: Hi 

}

void	displayNames(Server *server, Client &client, Message& msg)
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

void	join(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	//create new channel [#name] if it doesn't exist yet
	std::cout << " ----> i want to get into a channel!\n";
}

void	help(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// display standard stuff ...
	std::cout << " ----> what help?\n";
}

void	closeChannel(Server *server, Client &client, Message& msg)
{
	(void)server;
	(void)client;
	(void)msg;
	// leave channel, and if it's empty, remove it
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
