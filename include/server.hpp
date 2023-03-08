#ifndef __SERVER__HPP
#define __SERVER__HPP

#include "irc.hpp"

class Server
{
	private:
		int 		_sockfd;
		int			_portNum;
		std::string	_password;
		sockaddr_in	_server_addr;

	public:
		Server(int port, std::string pass);
		Server();
		~Server();
		
		int			getServerSoc(void) const;
		std::string	getPass(void) const;
};

#endif
