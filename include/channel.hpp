
#ifndef __CHANNEL_HPP
#define __CHANNEL_HPP

#include "irc.hpp"

class Channel
{
	private:
		std::list<Client>	_channelOperator;
		std::list<Client>	_channelUsers;
		std::string			_channelbuffer;
		std::string			_password;
		bool				_inviteOnly;

	public:
		Channel();
		~Channel();
};

void	connect();
void	join();
void	help();
void	closeChannel();
void	info();
void	whois();
void	changeNick();
void	msg();
void	displayNames();

#endif
