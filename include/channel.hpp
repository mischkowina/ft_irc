
#ifndef __CHANNEL_HPP
#define __CHANNEL_HPP

#include "irc.hpp"

class Client;
class Server;

class Channel
{
	private:
		std::list<Client>		_channelUsers;
		std::list<Client>		_bannedUsers;
		std::set<std::string>	_channelOperator;
		std::set<std::string>	_invitedUsers;
		std::set<std::string>	_voiceUsers;
		std::string				_channelName;
		std::string				_password;
		bool					_inviteOnly;
		bool					_secretChannel;
		bool					_privateChannel;
		bool					_quietChannel;
		bool					_moderatedChannel;
		bool					_supportChannelModes;
		bool					_topicChangeOnlyByChanop;
		bool					_anonymousChannel;
		bool					_noOutsideMessages;
		int						_userLimit;
		int						_userCounter;
		std::string				_topic;

	public:
		Channel();
		~Channel();
		Channel(std::string name);

		void	setChannelOp(Client &client);
		void	setChannelUsers(Client &client);
		void	setName(std::string channelName);
		void	setNewChannel(Client& client);
		void	setTopic(std::string topic);
		void	setInvite(char c);
		void	setSecret(char c);
		void	setPrivate(char c);
		void	setTopicChangeOnlyByChanop(char c);
		void	setModeratedChannel(char c);
		void	setPassWD(char c, std::string pass);
		void	setLimit(int limit);

		void	manageOperatorList(char c, std::string& nick);
		void	addClientToChannel(Server *server, Client &client, std::vector<std::string> &keys, int keyIndex);
		void	addToBannedList(Client& client, std::string list);
		void	addToVoiceList(Client& client);
		void	addToInviteList(std::string nick);

		bool	removeUser(Client& client);
		void	removeFromBannedList(std::string nick);
		void	removeFromVoiceList(std::string nick);
		void	removeFromInviteList(std::string nick);
		bool	validChannelName(Server *server, std::string& name, Client &client);
		void	setAnonymous(char c);
		void	setQuiet(char c);
		void	noOutsideMsg(char c);

		std::string				getTopic() const;
		std::string				getChannelName() const;
		std::set<std::string>	getChannelOperators() const;
		std::list<Client>		getChannelUsers() const;
		bool					isInviteOnly() const;
		bool					isSecret() const;
		bool					isPrivate() const;
		bool					isQuiet() const;
		bool					isModerated() const;
		bool					isTopicChangeOnlyByChanop() const;
		bool					supportedChannelModes() const;
		bool					isAnonymous() const;
		bool					allowsNoOutsideMessages() const;
		bool					clientIsChannelUser(std::string nick) const;
		bool					clientIsChannelOperator(std::string nick) const;
		bool					clientIsVoicedUser(std::string nick) const;

		std::list<Client>::iterator	getChannelUser(std::string nick);

		void	sendMsgToChannel(Client &sender, std::string msg, std::string type) const;
};

#endif
