#ifndef CMDS_HPP
# define CMDS_HPP

#include "irc.hpp"

class Server;
class Message;

void	connect(Server *server, Client &client, Message& msg);
void	join(Server *server, Client &client, Message& msg);
void	help(Server *server, Client &client, Message& msg);
void	closeChannel(Server *server, Client &client, Message& msg);
void	info(Server *server, Client &client, Message& msg);
void	whois(Server *server, Client &client, Message& msg);
void	nick(Server *server, Client &client, Message& msg);
void	privmsg(Server *server, Client &client, Message& msg);
void	displayNames(Server *server, Client &client, Message& msg);
void	pass_cmd(Server *server, Client &client, Message& msg);

#endif
