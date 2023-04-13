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

	if (changedClient.getName().empty() == false)
		changedClient.setIsAuthorized(true);

	if (parameters[0] == "horoscope" || parameters[0] == "anonymous" || server->addClient(changedClient) == false)
		client.sendErrMsg(server, ERR_NICKNAMEINUSE, parameters[0].c_str());
	else
	{
		if (changedClient.getName().empty() == false)
		{
			server->addAuthorizedClient(changedClient);
			std::string oldnick = client.getNick();
			if (ft::isValidNick(oldnick) == false)
				sendWelcome(server, changedClient);
		}
		server->eraseFromClientMap(client);
	}
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
	{
		client.setHasPass(true);
	}
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

	std::string nick = client.getNick();
	if (ft::isValidNick(nick) == true)
	{
		client.setIsAuthorized(true);
		server->addAuthorizedClient(client);
		sendWelcome(server, client);
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

void	oper(Server *server, Client &client, Message& msg)
{
	std::vector<std::string> parameters = msg.getParameters();

	if (parameters.size() < 2)
	{
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, "OPER");
		return ;
	}

	Server::ClientMap::iterator it = server->getAuthorizedClientMap().find(parameters[0]);

	if (it == server->getAuthorizedClientMap().end())
		return ;
	
	if (parameters[1] == server->getOperPass())
	{
		(*it).second.setIsOperator(true);
		Server::ClientMap::iterator itC = server->getClientMap().find((*it).second.getNick());
		(*itC).second.setIsOperator(true);
		(*it).second.sendErrMsg(server, RPL_YOUREOPER, NULL);//TBD: FUNCTION FOR REPLIES?
	}
	else
		client.sendErrMsg(server, ERR_PASSWDMISMATCH, NULL);
}

void	sendWelcome(Server *server, Client &client)
{
	std::string message = client.getNick() + "!" + client.getName() + "@" + client.getIP();
	client.sendErrMsg(server, RPL_WELCOME, message.c_str());
	client.sendErrMsg(server, RPL_YOURHOST, server->getHostname().c_str());
	client.sendErrMsg(server, RPL_CREATED, NULL);
	client.sendErrMsg(server, RPL_MYINFO, server->getHostname().c_str());
	Message msg("");
	motd(server, client, msg);
}
