#include "cmds.hpp"

/////////////////////////////////// MODE ////////////////////////////////////

void	info_all_user_modes(Server *server, Client &client)
{
	std::string	modes = "";

	if (client.isInvisible())
		modes += "i";
	if (client.getIsOperator())
		modes += "o";
	if (modes.empty() == false)
		modes.insert(0, "+");
	client.sendErrMsg(server, RPL_UMODEIS, modes.c_str());
}

void	info_all_channel_modes(Server *server, Client &client, Channel &channel)
{
	std::vector<std::string> params;
	params.push_back(channel.getChannelName());
	params.push_back("");
	params.push_back("");

	//if channel is anonymous
	if (channel.isAnonymous())
	{
		params[1] = "+a";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is n - no messages from outside
	if (channel.allowsNoOutsideMessages())
	{
		params[1] = "+n";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is quiet
	if (channel.isQuiet())
	{
		params[1] = "+q";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is secret
	if (channel.isSecret())
	{
		params[1] = "+s";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is private
	if (channel.isPrivate())
	{
		params[1] = "+p";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is moderated
	if (channel.isModerated())
	{
		params[1] = "+m";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is settable by operator only
	if (channel.isTopicChangeOnlyByChanop())
	{
		params[1] = "+t";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is invite
	if (channel.isInviteOnly())
	{
		params[1] = "+i";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel has a password
	if (channel.getChannelKey().empty() == false)
	{
		params[1] = "+k";
		params[2] = channel.getChannelKey() + " ";
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel is limited
	if (channel.getChannelLimit() > 0)
	{
		params[1] = "+l";
		std::stringstream ss;
		ss << channel.getChannelLimit();
		params[2] = ss.str();
		client.sendErrMsg(server, RPL_CHANNELMODEIS, params);
	}
	//if channel has ban mask - TODO
	if (channel.getChannelBanMasks().empty() == false)
	{
		params.pop_back();
		for (std::set<std::string>::iterator it = channel.getChannelBanMasks().begin(); it != channel.getChannelBanMasks().end(); it++)
		{
			params[1] = *it;
			client.sendErrMsg(server, RPL_BANLIST, params);
		}
		client.sendErrMsg(server, RPL_ENDOFBANLIST, channel.getChannelName().c_str());	
	}
}

void	userMode(Server *server, Client &client, std::vector<std::string>& parameters)
{
	//if first parameters is a nickname and it is not the same as the client issuing the message, send ERR msg -> clients are not allowed to change or view user modes for anyone but themselves
	if (parameters[0] != client.getNick()) {
		client.sendErrMsg(server, ERR_USERSDONTMATCH, NULL);
		return;
	}
	
	//if there is no second parameter, it's a query to see all active user modes
	if (parameters.size() < 2)
	{
		info_all_user_modes(server, client);
		return;
	}

	//check if second parameter is valid
	std::string options = parameters[1];
	std::string flags = options.substr(1);
	if ((options[0] != '+' && options[0] != '-') || flags.length() > 3 || flags.find_first_not_of("oi") != std::string::npos)
	{
		client.sendErrMsg(server, ERR_UMODEUNKNOWNFLAG, NULL);
		return;
	}

	//change stati for all flags in the second parameter
	for (size_t i = 0; i < flags.length(); i++)
	{
		bool status;
		if (options[0] == '+')
			status = true;
		else
			status = false;
		if (flags[i] == 'i')
		{
			if (client.setIsInvisible(status))
			{
				//set invisibility status in all channels of that user in case it changed
				for (Server::ChannelMap::iterator it = server->getChannelMap().begin(); it != server->getChannelMap().end(); it++)
				{
					if (it->second.clientIsChannelUser(client.getNick()))
						it->second.getChannelUser(client.getNick())->setIsInvisible(status);
				}
			}
		}
		if (flags[i] == 'o' && status == false)
			client.setIsOperator(false);
	}
	client.sendMsg(client, options, "MODE");
}

// void 	printAllValues(Server *server)
// {
// 	std::cout << "Present channels: " << std::endl;
// 	for (Server::ChannelMap::iterator it = server->getChannelMap().begin(); it != server->getChannelMap().end(); it++)
// 	{
// 		std::cout << it->second.getChannelName() << std::endl;
// 	}
// }

void	mode(Server *server, Client &client, Message& msg)
{	
	std::vector<std::string> parameters = msg.getParameters();
	if (parameters.empty()) {
		client.sendErrMsg(server, ERR_NEEDMOREPARAMS, NULL);
		return;
	}
	std::string& channel = parameters[0];
	std::transform(channel.begin(), channel.end(), channel.begin(), ::tolower);

	//check if parameter[0] is a channel or a user - if user, call usermode function, if none send error, else continue with this function
	Server::ChannelMap::iterator itChannel = server->getChannelMap().find(channel);
	if (server->getAuthorizedClientMap().find(channel) != server->getAuthorizedClientMap().end())
	{
		userMode(server, client, parameters);
		return;
	}
	else if (itChannel == server->getChannelMap().end())
	{
		if (channel.find_first_of("+#&") != std::string::npos)
			client.sendErrMsg(server, ERR_NOSUCHCHANNEL, channel.c_str());
		else
			client.sendErrMsg(server, ERR_NOSUCHNICK, parameters[0].c_str());
		return;
	}

	//if there is only one (valid) parameter, send info message on all modes of the channel
	if (parameters.size() < 2) {
		info_all_channel_modes(server, client, server->getChannelMap().find(channel)->second);
		return;
	}

		// MODE <channel> <flags> [<args>]
	std::set<std::string> operators = itChannel->second.getChannelOperators();
	std::string options = parameters[1];
	std::string flags = options.substr(1);
	if ((options[0] != '+' && options[0] != '-') || flags.length() > 3 || flags.find_first_not_of("aopsitqmnbvkl") != std::string::npos)
	{ 
		client.sendErrMsg(server, ERR_UNKNOWNMODE, flags.data());
		return;
	}
	if (operators.find(client.getNick()) == operators.end()) {
		client.sendErrMsg(server, ERR_NOPRIVILEGES, NULL);
		return;
	}
	if (itChannel->second.supportedChannelModes() == false && flags == "t") {
		itChannel->second.setTopicChangeOnlyByChanop(options[0]);
		return;
	}

	///////////////////////////////////////

	// +vbl [<limit>] [<user>] [<ban mask>]
	std::string limit;
	std::string user;
	std::string addBanList;

	if (parameters.size() > 2)
	{
		std::stringstream ss;
		if (parameters.size() > 2) {
			ss << parameters[2] << " ";
		}
		if (parameters.size() > 3) {
			ss << parameters[3] << " ";
		}
		if (parameters.size() > 4) {
			ss << parameters[4] << " ";
		}
		if (flags.find_first_of('l') != std::string::npos){
			ss >> limit;
		}
		if (flags.find_first_not_of("lb") != std::string::npos){
			ss >> user;
		}
		if (flags.find_first_of('b') != std::string::npos){
			ss >> addBanList;
		}
	}

	const char* tmpCStr = flags.c_str();
	for (unsigned int i = 0; i < flags.length(); i++)
	{
		//check for o / v option if given user exists / is on the channel
		if (tmpCStr[i] == 'o' || tmpCStr[i] == 'v')
		{
			//check if user exists on the server
			if (server->getAuthorizedClientMap().find(user) == server->getAuthorizedClientMap().end())
			{
				client.sendErrMsg(server, ERR_NOSUCHNICK, user.c_str());
				continue ;
			}
			//check if user is on the channel
			if (itChannel->second.clientIsChannelUser(user) == false)
			{
				std::vector<std::string> params;
				params.push_back(user);
				params.push_back(itChannel->second.getChannelName());
				client.sendErrMsg(server, ERR_USERNOTINCHANNEL, params);
				continue ;
			}
		}
		
		std::string message;
		message.push_back(options[0]);
		message.push_back(tmpCStr[i]);

		switch (tmpCStr[i])
		{
			case 'o':
				itChannel->second.manageOperatorList(options[0], user);
				message.append(" " + user);
				break;
			case 'b':
				itChannel->second.manageBanList(server, client, options[0], addBanList);
				if (addBanList == "")
					continue;
				message.append(" " + addBanList);
				break;
			case 'v':
				itChannel->second.manageVoiceList(options[0], user);
				message.append(" " + user);
				break;
			case 'k':
				itChannel->second.setPassWD(options[0], user);
				message.append(" " + user);
				break;
			case 'i':
				itChannel->second.setInvite(options[0]);
				break;
			case 'p':
				itChannel->second.setPrivate(options[0]);
				break;
			case 's':
				itChannel->second.setSecret(options[0]);
				break;
			case 't':
				itChannel->second.setTopicChangeOnlyByChanop(options[0]);
				break;
			case 'm':
				itChannel->second.setModeratedChannel(options[0]);
				break;
			case 'l':
				itChannel->second.setLimit(options[0], limit);
				message.append(" " + limit);
				break;
			case 'n':
				itChannel->second.setOutsideMsg(options[0]);
				break;
			case 'q':
				itChannel->second.setQuiet(options[0]);
				break;
			case 'a':
				itChannel->second.setAnonymous(options[0]);
				break;
		}
		itChannel->second.sendMsgToChannel(client, message, "MODE");
	}
}
