#include "cmds.hpp"

void	ping(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.empty() == true)
	{
		client.sendErrMsg(server, ERR_NOORIGIN, NULL);
		return ;
	}

	std::string message = ":" + server->getHostname() + " PONG " + msg.getParameters().at(0);
	std::cout << RED "Sending to " << client.getNick() << ": " RESET << message << std::endl;
	message.append("\r\n");
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
