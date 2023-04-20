#include "cmds.hpp"

/////////////////////////////////// KICK ////////////////////////////////////

void	kick_client_from_channel(Server *server, Client &client, Channel &channel, std::vector<std::string>	parameters, Client &victim)
{
	std::string	msg = victim.getNick() + " :";

	//KICK message contains the victims name + the reason for getting kicked (if given) or the name of the person that kicked
	if (parameters.size() > 2)
		msg.append(parameters[2]);
	else
		msg.append(client.getNick());
	
	//send message to the whole channel that the user got kicked from (still including that user) - unless it's a quiet channel
	if (channel.isQuiet() == false)
		channel.sendMsgToChannel(client, msg, "KICK");

	//actually remove the victim from the channel
	channel.removeUser(victim, "", "PART");
	if (channel.getChannelUsers().empty())
		server->removeChannel(channel.getChannelName());
}

void	kick(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();
	
	if (parameters.size() < 2)
	{
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, "KICK");
		return ;
	}

	std::vector<std::string> channelNames;
	std::stringstream ss(parameters[0]);
	std::string token;
	while (std::getline(ss, token, ',')) {
		std::transform(token.begin(), token.end(), token.begin(), ::tolower);
		channelNames.push_back(token);
		token.clear();
	}

	std::vector<std::string> clientNames;
	std::stringstream ss2(parameters[1]);
	std::string token2;
	while (std::getline(ss2, token2, ',')) {
		clientNames.push_back(token2);
		token2.clear();
	}

	//either there is only one channel name with multiple users to be kicked, or one channel name per user name
	if (channelNames.size() > 1 && channelNames.size() != clientNames.size())
	{
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, "KICK");
		return ;
	}
	
	//go through all channels and remove the respective clients
	for (size_t i = 0; i < channelNames.size(); i++)
	{
		//check if channel exists
		if (server->getChannelMap().find(channelNames[i]) == server->getChannelMap().end())
		{
			client.sendErrMsg(server, ERR_NOSUCHCHANNEL, channelNames[i].c_str());
			continue;
		}

		//check if client is on that channel
		Channel channel = server->getChannelMap().find(channelNames[i])->second;
		if (channel.clientIsChannelUser(client.getNick()) == false)
		{
			client.sendErrMsg(server, ERR_NOTONCHANNEL, channelNames[i].c_str());
			continue;
		}

		//check if client is chanop and allowed to kick others
		if (channel.clientIsChannelOperator(client.getNick()) == false)
		{
			client.sendErrMsg(server, ERR_CHANOPRIVSNEEDED, channelNames[i].c_str());
			continue;
		}

		//check if one or more users are supposed to be kicked
		if (channelNames.size() == 1 && clientNames.size() > 1)
		{
			for (size_t j = 0; j < clientNames.size(); j++)
			{
				//check if the user to be kicked is in the channel
				if (channel.clientIsChannelUser(clientNames[j]) == false)
				{
					std::vector<std::string> params;
					params.push_back(clientNames[j]);
					params.push_back(channel.getChannelName());
					client.sendErrMsg(server, ERR_USERNOTINCHANNEL, params);
					continue;
				}

				//send message with KICK information to victim
				Client victim = *(channel.getChannelUser(clientNames[j]));
				kick_client_from_channel(server, client, channel, parameters, victim);
			}
			return ;
		}

		//else kick out one user each from multiple channels

		//check if the user to be kicked is in the channel
		if (channel.clientIsChannelUser(clientNames[i]) == false)
		{
			std::vector<std::string> params;
			params.push_back(clientNames[i]);
			params.push_back(channel.getChannelName());
			client.sendErrMsg(server, ERR_USERNOTINCHANNEL, params);
			continue;
		}

		//send message with KICK information to victim
		Client victim = *(channel.getChannelUser(clientNames[i]));
		kick_client_from_channel(server, client, channel, parameters, victim);
	}
}
