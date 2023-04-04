#ifndef CMDS_HPP
# define CMDS_HPP

#include "irc.hpp"

class Server;
class Message;

void	nick(Server *server, Client &client, Message& msg);
void	pass_cmd(Server *server, Client &client, Message& msg);
void	user(Server *server, Client &client, Message& msg);
void	quit(Server *server, Client &client, Message& msg);
void	oper(Server *server, Client &client, Message& msg);
void	sendWelcome(Server *server, Client &client);

void	join(Server *server, Client &client, Message& msg);
void	part(Server *server, Client &client, Message& msg);
void	topic(Server *server, Client &client, Message& msg);

void	motd(Server *server, Client &client, Message& msg);

void	privmsg(Server *server, Client &client, Message& msg);
void	notice(Server *server, Client &client, Message& msg);

void	ping(Server *server, Client &client, Message& msg);
void	pong(Server *server, Client &client, Message& msg);

void	die(Server *server, Client &client, Message& msg);



#endif
