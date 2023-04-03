#include "cmds.hpp"

void	motd(Server *server, Client &client, Message& msg)
{
	(void)msg;
	
	client.sendErrMsg(server, RPL_MOTDSTART, server->getHostname().c_str());
	client.sendErrMsg(server, RPL_MOTD, "Whatever");
	client.sendErrMsg(server, RPL_ENDOFMOTD, NULL);
}
