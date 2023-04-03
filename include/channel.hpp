
#ifndef __CHANNEL_HPP
#define __CHANNEL_HPP

#include "irc.hpp"

class Client;

class Channel
{
	private:
		std::list<Client>	_channelOperator;
		std::list<Client>	_channelUsers;
		std::string			_channelName;
		std::string			_channelbuffer;
		std::string			_password;
		bool				_inviteOnly;

	public:
		Channel();
		~Channel();
		Channel(std::string name);

		void	setName(std::string channelName);
		void	setNewChannel(Client& client);

		std::string			getChannelName() const;
		std::list<Client>	getChannelOperator() const;
		std::list<Client>	getChannelUsers() const;
};

#endif
