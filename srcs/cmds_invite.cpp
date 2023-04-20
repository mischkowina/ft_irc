#include "cmds.hpp"

/////////////////////////////////// INVITE ////////////////////////////////////

void	invite(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();
	Server::ClientMap			clients = server->getAuthorizedClientMap();
	Server::ChannelMap			channels = server->getChannelMap();

	//check if enough parameters
	if (parameters.size() < 2)
	{
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, "INVITE");
		return ;
	}

	//check if the client specified by the first parameter exists
	Server::ClientMap::iterator receiver = clients.find(parameters[0]);
	if (receiver == clients.end())
	{
		client.sendErrMsg(server, ERR_NOSUCHNICK, parameters[0].c_str());
		return ;
	}

	//check if the channel exists and if so, if client is on the channel 
	std::transform(parameters[1].begin(), parameters[1].end(), parameters[1].begin(), ::tolower);
	Server::ChannelMap::iterator itChan = channels.find(parameters[1]);
	if (itChan == channels.end() || itChan->second.clientIsChannelUser(client.getNick()) == false)
	{
		client.sendErrMsg(server, ERR_NOTONCHANNEL, parameters[1].c_str());
		return ;
	}

	//check if channel is invite only and if so, if client has chanop priviliges
	if (itChan->second.isInviteOnly() == true && itChan->second.clientIsChannelOperator(client.getNick()) == false)
	{
		client.sendErrMsg(server, ERR_CHANOPRIVSNEEDED, parameters[1].c_str());
		return ;
	}

	//check if the invited client is already on that channel
	if (itChan->second.clientIsChannelUser(parameters[0]) == true)
	{
		std::vector<std::string> params;
		params.push_back(parameters[0]);
		params.push_back(parameters[1]);
		client.sendErrMsg(server, ERR_USERONCHANNEL, params);
		return ;
	}

	//add the name of the invited client to the invite list of the channel - to be checked for the nick while joining when the channel is invite only
	itChan->second.addToInviteList(parameters[0]);

	//send message to client that has been invited
	receiver->second.sendMsg(client, parameters[1], "INVITE");

	//send reply to the client that invited to signal succesful invitation
	std::vector<std::string> params;
	params.push_back(parameters[1]);
	params.push_back(parameters[0]);
	client.sendErrMsg(server, RPL_INVITING, params);
	if (receiver->second.getAwayMsg().empty() == false)
	{
		std::vector<std::string> params;
		params.push_back(receiver->second.getNick());
		params.push_back(receiver->second.getAwayMsg());
		client.sendErrMsg(server, RPL_AWAY, params);
	}
}
