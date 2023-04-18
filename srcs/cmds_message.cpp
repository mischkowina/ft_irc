#include "cmds.hpp"

void findReceivers(Server *server, Client &sender, std::vector<std::string> listOfRecv, std::string msg)
{
	Server::ClientMap tmpClient = server->getAuthorizedClientMap();
	Server::ChannelMap tmpChannel = server->getChannelMap();

	// iterate through receivers, try to find a matching channel or client to send the messag eto
	for (std::vector<std::string>::const_iterator itRecv = listOfRecv.begin(); itRecv != listOfRecv.end(); itRecv++)
	{
		Server::ClientMap::iterator itClien = tmpClient.find(*itRecv);
		if (itClien != tmpClient.end())
		{
			itClien->second.sendMsg(sender, msg, "PRIVMSG");
			if (itClien->second.getAwayMsg().empty() == false)
			{
				std::vector<std::string> params;
				params.push_back(itClien->second.getNick());
				params.push_back(itClien->second.getAwayMsg());
				sender.sendErrMsg(server, RPL_AWAY, params);
			}
			continue ;
		}
		Server::ChannelMap::const_iterator itChannel = tmpChannel.find(*itRecv);
		if (itChannel != tmpChannel.end())
		{
			//sending message not possible if the channel is moderated and the sender is neither voiced nor chanop
			if (itChannel->second.isModerated() && (itChannel->second.clientIsVoicedUser(sender.getNick()) == false) && itChannel->second.clientIsChannelOperator(sender.getNick()) == false)
				sender.sendErrMsg(server, ERR_CANNOTSENDTOCHAN, itChannel->second.getChannelName().c_str());
			//sending message not possible if the channel allows no outside messages and the sender is not on the channel
			else if (itChannel->second.allowsNoOutsideMessages() && itChannel->second.clientIsChannelUser(sender.getNick()) == false)
				sender.sendErrMsg(server, ERR_CANNOTSENDTOCHAN, itChannel->second.getChannelName().c_str());
			//sending message not possible if the sender is banned on that channel
			else if (itChannel->second.clientIsBanned(sender.getNick()))
				sender.sendErrMsg(server, ERR_CANNOTSENDTOCHAN, itChannel->second.getChannelName().c_str());
			else
				itChannel->second.sendMsgToChannel(sender, msg, "PRIVMSG");
			continue ;
		}
		//check if message is directed at horoscope bot
		if (*itRecv == "horoscope")
		{
			horoscope(server, sender, msg);
			continue;
		}
		sender.sendErrMsg(server, ERR_NOSUCHNICK, itRecv->c_str());
	}
}

// send (private) msg to a specific user/channel
void	privmsg(Server *server, Client &client, Message& msg)
{
	// Parameters: <receiver>{,<receiver>} <text to be sent>

	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.empty() == true)
	{
		client.sendErrMsg(server, ERR_NORECIPIENT, "PRIVMSG");
		return ;
	}

	if (parameters.size() < 2)
	{
		client.sendErrMsg(server, ERR_NOTEXTTOSEND, NULL);
		return ;
	}

	std::vector<std::string>	receivers;
	std::string textToBeSend = parameters[1];

	//parse first parameter by commas to get all the recipients -> <receiver> can be a list of names or channels
	std::stringstream ss(parameters[0]);
	std::string token;
	while (std::getline(ss, token, ',')) {
		receivers.push_back(token);
		token.clear();
	}

	//send message to all receivers
	try {
		findReceivers(server, client, receivers, textToBeSend);
	} catch (const std::exception& e) {
		std::cerr << "--- Error --- : " << e.what() << std::endl;	// adjust appropriate response here
	}
}

void	notice(Server *server, Client &client, Message& msg)
{
	std::vector<std::string>	parameters = msg.getParameters();

	if (parameters.size() < 2)
		return;

	Server::ClientMap::iterator it = server->getAuthorizedClientMap().find(parameters[0]);
	if (it != server->getAuthorizedClientMap().end())
	{
		(*it).second.sendMsg(client, parameters[1], "NOTICE");
		return;
	}

	Server::ChannelMap::iterator itChan = server->getChannelMap().find(parameters[0]);
	if (itChan != server->getChannelMap().end())
		(*itChan).second.sendMsgToChannel(client, parameters[1], "NOTICE");
}
