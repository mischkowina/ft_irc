
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
		std::list<Client>	_voiceUsers;
		std::string			_channelName;
		std::string			_channelbuffer;
		std::string			_password;
		bool					_inviteOnly;
		bool					_secretChannel;
		bool					_privateChannel;
		bool					_quietChannel;
		bool					_moderatedChannel;
		bool					_supportChannelModes;
		bool					_changeTopic;
		int						_userLimit;
		std::string				_topic;
		std::set<std::string>	_inviteList;

	public:
		Channel();
		~Channel();
		Channel(std::string name);

		void	setChannelOp(Client &client);
		void	setChannelUsers(Client &client);
		void	setName(std::string channelName);
		void	setNewChannel(Client& client);
		void	setTopic(std::string topic);
		void	setInvite(bool arg);
		void	setSecret(bool arg);
		void	setPrivate(bool arg);
		void	setChangeTopic(bool arg);
		void	setModeratedChannel(bool arg);
		void	setPassWD(std::string pass);
		void	setLimit(int limit);

		void	addClientToChannel(Server *server, Client &client, std::vector<std::string> &keys, int keyIndex);
		void	addClientToInviteList(std::string nick);
		void	addClientToOperatorList(Client &client);
		void	addToBannedList(Client& client);
		void	addToVoiceList(Client& client);

		bool	removeUser(Client& client);
		void	removeFromOperatorList(std::string nick);
		void	removeFromBannedList(std::string nick);
		void	removeFromVoiceList(std::string nick);

		std::string			getTopic() const;
		std::string			getChannelName() const;
		std::list<Client>	getChannelOperator() const;
		std::list<Client>	getChannelUsers() const;
		bool				isInviteOnly() const;
		bool				supportChannelModes() const;
		bool				clientIsChannelUser(std::string nick) const;
		bool				clientIsChannelOperator(std::string nick) const;
		std::list<Client>::iterator	getChannelUser(std::string nick);

};

#endif
