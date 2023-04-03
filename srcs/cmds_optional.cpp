#include "cmds.hpp"

void	die(Server *server, Client &client, Message& msg)
{
	(void)msg;
	
	if (client.getIsOperator() == false)
	{
		client.sendErrMsg(server, ERR_NOPRIVILEGES, NULL);
		return ;
	}

	Server::ClientMap::iterator it = server->getClientMap().begin();
	while (it != server->getClientMap().end())
		close(it->second.getSocket());

	server->setHasDied(true);
}
