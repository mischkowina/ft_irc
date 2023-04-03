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
			(*itClien).second.sendMsg(sender, msg, "PRIVMSG");
			continue ;
		}
		Server::ChannelMap::const_iterator itChannel = tmpChannel.find(*itRecv);
		if (itChannel != tmpChannel.end())
		{
			std::list<Client> tmpChannelUsers = itChannel->second.getChannelUsers();
			for (std::list<Client>::const_iterator itChannelRecv = tmpChannelUsers.begin(); itChannelRecv != tmpChannelUsers.end(); itChannelRecv++)
				(*itChannelRecv).sendMsg(sender, msg, "PRIVMSG");
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

void	notice(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.size() < 2)
		return;

	Server::ClientMap::iterator it = server->getAuthorizedClientMap().find(parameters[0]);
	if (it == server->getAuthorizedClientMap().end())
		return;

	(*it).second.sendMsg(client, parameters[1], "NOTICE");
}

void	ping(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.empty() == true)
	{
		client.sendErrMsg(server, ERR_NOORIGIN, NULL);
		return ;
	}

	std::string message = ":" + server->getHostname() + " PONG " + msg.getParameters().at(0) + "\r\n";
	send(client.getSocket(), message.data(), message.length(), 0);
}

void	pong(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.empty() == true)
	{
		client.sendErrMsg(server, ERR_NOORIGIN, NULL);
		return ;
	}
	//DOESN'T DO ANYTHING, JUST TO BE ABLE TO HANDLE PONG MESSAGES FROM CLIENTS
}