#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "irc.hpp"

class Client;
class Server;

class Channel
{
	private:
		std::list<Client>		_channelUsers;
		std::set<std::string>	_channelOperator;
		std::set<std::string>	_banList;
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
		void	setLimit(char c, std::string limit);

		void	manageOperatorList(char c, std::string nick);
		void	manageVoiceList(char c, std::string& nick);
		void	manageBanList(Server *server, Client& client, char c, std::string& banMask);
		void	manageInviteList(char c, std::string& nick);
		void	addClientToChannel(Server *server, Client &client, std::vector<std::string> &keys, int keyIndex);

		void	removeFromOperatorList(std::string nick);
		void	addToVoiceList(Client &client);
		void	addToInviteList(std::string nick);
		void	removeFromInviteList(std::string nick);
		void	removeFromVoiceList(std::string nick);

		bool	removeUser(Client& client, std::string message, std::string type);
		void	setAnonymous(char c);
		void	setQuiet(char c);
		void	setOutsideMsg(char c);
		bool	includedOnBanList(Client& client) const;
		
		std::string				getTopic() const;
		std::string				getChannelName() const;
		std::set<std::string>	&getChannelOperators();
		std::list<Client>		&getChannelUsers();
		std::string				getChannelKey() const;
		int						getChannelLimit() const;
		std::set<std::string>	&getChannelBanMasks();
		std::set<std::string>	&getChannelInviteList();
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
		bool					clientIsInvited(std::string nick) const;

		std::list<Client>::iterator	getChannelUser(std::string nick);
		void						updateNick(Client &oldNick, Client &newNick);

		void	sendMsgToChannel(Client &sender, std::string msg, std::string type) const;
};

bool	validChannelName(Server *server, std::string& name, Client &client);

#endif
