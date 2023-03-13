#ifndef __SERVER__HPP
# define __SERVER__HPP

# include "irc.hpp"

typedef void (*FuncPtr)(Server *server, Client *client, Message& msg);

class Server
{
	private:
		int 			_sockfd;
		int				_portNum;
		std::string		_password;
		sockaddr_in		_server_addr;
		ft::ClientMap	_clients;
		ft::ChannelMap	_channels;
		std::map<std::string, FuncPtr> _cmdMap;

	public:
		Server(int port, std::string pass);
		Server();
		~Server();
		
		int			getServerSoc(void) const;
		std::string	getPass(void) const;

		void	run();

		void	checkAllClientSockets(std::vector<pollfd> pollfds);
		void	checkListeningSocket(std::vector<pollfd> pollfds);
		void	process_request(Client *client, std::string msg);
		
		void	runCmd(Client *client, Message& msg);

};

/* 
 * functions to comunicate with clients
 */

void	connect(Server *server, Client *client, Message& msg);
void	join(Server *server, Client *client, Message& msg);
void	help(Server *server, Client *client, Message& msg);
void	closeChannel(Server *server, Client *client, Message& msg);
void	info(Server *server, Client *client, Message& msg);
void	whois(Server *server, Client *client, Message& msg);
void	changeNick(Server *server, Client *client, Message& msg);
void	sendMsg(Server *server, Client *client, Message& msg);
void	displayNames(Server *server, Client *client, Message& msg);

#endif
