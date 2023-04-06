#include "cmds.hpp"

void	away(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.empty() == true)
	{
		client.setAwayMsg("");
		Server::ClientMap::iterator it = server->getAuthorizedClientMap().find(client.getNick());
		it->second.setAwayMsg("");
		client.sendErrMsg(server, RPL_UNAWAY, NULL);
	}
	else
	{
		client.setAwayMsg(parameters[0]);
		Server::ClientMap::iterator it = server->getAuthorizedClientMap().find(client.getNick());
		it->second.setAwayMsg(parameters[0]);
		client.sendErrMsg(server, RPL_NOWAWAY, NULL);
	}
}

void	die(Server *server, Client &client, Message& msg)
{
	(void)msg;
	
	if (client.getIsOperator() == false)
	{
		client.sendErrMsg(server, ERR_NOPRIVILEGES, NULL);
		return ;
	}

	server->setHasDied(true);
}
