
#ifndef __CHANNEL_HPP
#define __CHANNEL_HPP

#include "irc.hpp"

class Client;

class Channel
{
	private:
		std::list<Client>		_channelOperator;
		std::list<Client>		_channelUsers;
		std::string				_channelName;
		std::string				_channelbuffer;
		std::string				_password;
		bool					_inviteOnly;
		std::string				_topic;
		std::set<std::string>	_inviteList;

	public:
		Channel();
		~Channel();
		Channel(std::string name);

		void	setName(std::string channelName);
		void	setNewChannel(Client& client);
		void	setTopic(std::string topic);
		void	addClientToInviteList(std::string nick);
		bool	removeUser(Client& client);

		std::string			getChannelName() const;
		std::list<Client>	getChannelOperator() const;
		std::list<Client>	getChannelUsers() const;
		bool				isInviteOnly() const;
		std::string			getTopic() const;
		bool				clientIsChannelUser(std::string nick) const;
		bool				clientIsChannelOperator(std::string nick) const;
		std::list<Client>::iterator	getChannelUser(std::string nick);
};

#endif
