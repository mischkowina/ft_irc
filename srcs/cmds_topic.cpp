#include "cmds.hpp"

/////////////////////////////////// TOPIC ////////////////////////////////////

void	topic(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.empty() == true)
	{
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, "TOPIC");
		return ;
	}

	//check if channel exists
	std::string channel = parameters[0];
	std::transform(channel.begin(), channel.end(), channel.begin(), ::tolower);
	Server::ChannelMap::iterator it = server->getChannelMap().find(channel);
	if (it == server->getChannelMap().end())
	{
		client.sendErrMsg(server, ERR_NOTONCHANNEL, channel.c_str());
		return;
	}

	// if only one parameter, TOPIC is requested
	if (parameters.size() == 1)
	{
		//if channel is private/secret and client is not on, return ERR_NOTONCHANNEL
		if ((it->second.isPrivate() || it->second.isSecret()) && it->second.clientIsChannelUser(client.getNick()) == false)
		{
			client.sendErrMsg(server, ERR_NOTONCHANNEL, channel.c_str());
			return ;
		}
		//if there is a topic, return RPL_TOPIC
		if (it->second.getTopic() != "")
		{
			std::vector<std::string>	msg_input;
			msg_input.push_back(channel);
			msg_input.push_back(it->second.getTopic());
			client.sendErrMsg(server, RPL_TOPIC, msg_input);
		}
		//else return RPL_NOTOPIC
		else
			client.sendErrMsg(server, RPL_NOTOPIC, channel.c_str());
	}

	//if two parameters, topic shall be changed
	else
	{
		//can only change topic if on the channel
		if (it->second.clientIsChannelUser(client.getNick()) == false)
		{
			client.sendErrMsg(server, ERR_NOTONCHANNEL, channel.c_str());
			return ;
		}
		//if channelmode +t, only chanops can change the topic
		if (it->second.isTopicChangeOnlyByChanop() && it->second.clientIsChannelOperator(client.getNick()) == false)
		{
			client.sendErrMsg(server, ERR_CHANOPRIVSNEEDED, it->second.getChannelName().c_str());
			return ;
		}
		it->second.setTopic(parameters[1]);
	}
}
