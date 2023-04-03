#include "cmds.hpp"

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
