#ifndef __SERVER__HPP
# define __SERVER__HPP

# include "irc.hpp"

class Server
{
	private:
		int 			_sockfd;
		int				_portNum;
		std::string		_password;
		sockaddr_in		_server_addr;
		ft::ClientMap	_clients;
		// std::map<std::string, Channel>	_channels;

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
};

/* 
 * functions to comunicate with clients
 */
typedef void (*FuncPtr)();


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
