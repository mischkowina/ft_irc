
#ifndef __CHANNEL_HPP
#define __CHANNEL_HPP

#include "irc.hpp"

class Client;
class Server;

class Channel
{
	private:
		std::list<Client>	_channelOperator;
		std::list<Client>	_channelUsers;
		std::list<Client>	_bannedUsers;
		std::list<Client>	_invitedUsers;
		std::string			_channelName;
		std::string			_channelbuffer;
		std::string			_password;
		bool				_inviteOnly;

	public:
		Channel();
		~Channel();
		Channel(std::string name);

		void	setChannelOp(Client &client);
		void	setChannelUsers(Client &client);
		void	setName(std::string channelName);
		
		void	addClientToChannel(Server *server, Client &client, std::vector<std::string> &keys, int keyIndex);

		std::string			getChannelName() const;
		std::list<Client>	getChannelOperator() const;
		std::list<Client>	getChannelUsers() const;
};

#endif
