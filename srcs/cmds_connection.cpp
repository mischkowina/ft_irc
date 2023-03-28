#include "cmds.hpp"

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
	{
		if (changedClient.getName().empty() == false)
		{
			changedClient.setIsAuthorized(true);
			server->addAuthorizedClient(changedClient);
		}	
		server->eraseFromClientMap(client);
	}
	//COMMENT: No implementation of ERR_NICKCOLLISION since it is only applicable for multi-server connections
}

/////////////////////////////////// PASS ////////////////////////////////////

void	pass_cmd(Server *server, Client &client, Message& msg)
{
	std::vector<std::string> parameters = msg.getParameters();

	//check if client is already authorized (already used PASS before)
	if (client.getHasPass() == true)
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
		client.setHasPass(true);
}

/////////////////////////////////// USER ////////////////////////////////////

void	user(Server *server, Client &client, Message& msg)
{
	std::vector<std::string> parameters = msg.getParameters();

	if (client.getName().empty() != true)
	{
		client.sendErrMsg(server, ERR_ALREADYREGISTRED, NULL);
		return ;
	}

	if (parameters.size() < 4)
	{
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, "USER");
		return ;
	}

	client.setUserData(parameters);

	if (client.getNick().empty() == false)
	{
		client.setIsAuthorized(true);
		server->addAuthorizedClient(client);
	}	
}

/////////////////////////////////// QUIT ////////////////////////////////////

void	quit(Server *server, Client &client, Message& msg)
{
	std::vector<std::string> parameters = msg.getParameters();
	std::string	message = ":" + server->getHostname() + " QUIT ";

	if (parameters.empty() == true)
		message.append(client.getNick());
	else
		message.append(parameters[0]);

	send(client.getSocket(), message.data(), message.length(), 0);
	close(client.getSocket());
	server->eraseFromClientMap(client);
}

/////////////////////////////////// OPER ////////////////////////////////////

// void	oper(Server *server, Client &client, Message& msg)
// {

// }