#ifndef __SERVER__HPP
# define __SERVER__HPP

# include "irc.hpp"

typedef void (*FuncPtr)(Server *server, Client &client, Message& msg);

class Server
{
	public:

		typedef std::map<std::string, class Client>		ClientMap;
		typedef std::map<std::string, class Channel>	ChannelMap;

		Server(int port, std::string pass);
		Server();
		~Server();
		
		int			getServerSoc(void) const;
		std::string	getHostname() const;
		std::string	getPass(void) const;
		ClientMap	getClientMap() const;
		ClientMap	getAuthorizedClientMap() const;
		ChannelMap	getChannelMap() const;

		void		eraseFromClientMap(Client &client);
		bool		addClient(Client &client);
		void		addAuthorizedClient(Client &client);

		void	run();

		void	checkAllClientSockets(std::vector<pollfd> pollfds);
		void	checkListeningSocket(std::vector<pollfd> pollfds);
		void	process_request(Client &client, std::string msg);

		void	execCmd(Client &client, Message& msg);
	
	private:
		int 			_sockfd;
		int				_portNum;
		std::string		_hostname;
		std::string		_password;
		sockaddr_in		_server_addr;
		ClientMap		_clients;
		ChannelMap		_channels;
		ClientMap		_authorizedClients;//only contains the clients that are authenticated and allowed to interact on the server
		std::map<std::string, FuncPtr> _cmdMap;
		bool			_noAuthorization;
		bool			_clientMapChanged;
};

/* 
 * functions to comunicate with clients
 */

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
void	user(Server *server, Client &client, Message& msg);
void	quit(Server *server, Client &client, Message& msg);

#endif
