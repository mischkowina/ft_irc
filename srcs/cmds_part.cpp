#include "cmds.hpp"

//////////////////////////////  PART  ////////////////////////////////////////

void	part(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.empty() == true)
	{
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, "PART");
		return ;
	}
	
	std::vector<std::string> channelNames;

	//parse first parameter by commas to get all channel names
	std::stringstream ss(parameters[0]);
	std::string token;
	while (std::getline(ss, token, ',')) {
		channelNames.push_back(token);
		token.clear();
	}

	std::string	message;
	if (parameters.size() > 1)
		message = parameters[1];
	else 
		message = client.getNick();

	Server::ChannelMap::iterator it;
	for (size_t i = 0; i < channelNames.size(); i++)
	{
		it = server->getChannelMap().find(channelNames[i]);
		if (it != server->getChannelMap().end())
		{
			if (it->second.removeUser(client) == false)
				client.sendErrMsg(server, ERR_NOTONCHANNEL, channelNames[i].c_str());
			else
			{
				if (it->second.isQuiet())
					continue ;
				if (it->second.isAnonymous() && message == client.getNick())
					it->second.sendMsgToChannel(client, "anonymous", "PART");
				else
					it->second.sendMsgToChannel(client, message, "PART");
			}	
		}
		else 
			client.sendErrMsg(server, ERR_NOSUCHCHANNEL, channelNames[i].c_str());
	}
}
