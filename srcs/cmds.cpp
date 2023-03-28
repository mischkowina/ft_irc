#include "cmds.hpp"

void findReceivers(Server *server, Client &sender, std::vector<std::string> listOfRecv, std::string msg)
{
	Server::ClientMap tmpClient = server->getAuthorizedClientMap();
	Server::ChannelMap tmpChannel = server->getChannelMap();

	// iterate through receivers, try to find a matching channel or client to send the messag eto
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
